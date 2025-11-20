from __future__ import annotations

import asyncio
import logging
import time
from dataclasses import dataclass, field
from typing import Any, Dict, List, Optional, Sequence

import g4f

from llm import BandwidthPolicy

logger = logging.getLogger(__name__)

GLOBAL_BANDWIDTH_POLICY = BandwidthPolicy(limit_per_minute=4000)


@dataclass
class ProviderTestResult:
    """Provider 测试结果"""
    name: str
    success: bool
    duration: Optional[float]
    error: Optional[str] = None


@dataclass
class ProviderSelectionResult:
    """Provider 选择结果"""
    best_provider: Any
    tests: List[ProviderTestResult]


@dataclass
class ProviderConfig:
    """Provider 配置"""
    names: Sequence[str]
    restricted_names: Sequence[str]
    stable_name: str
    benchmark_prompt: str = "hi"
    benchmark_timeout: int = 15
    health_check_interval: int = 60  # 健康检查间隔（秒）


@dataclass
class WeakNetworkState:
    """弱网状态"""
    active: bool = False
    reason: Optional[str] = None
    activated_at: Optional[float] = None
    success_counter: int = 0
    required_successes: int = 2  # 需要连续成功的次数才能退出弱网模式


@dataclass
class ProviderHealth:
    """Provider 健康状态"""
    name: str
    is_healthy: bool = True
    last_check: Optional[float] = None
    failure_count: int = 0
    success_count: int = 0
    max_failures: int = 3  # 最大失败次数


class ProviderManager:
    """Provider 管理器 - 负责 provider 的选择、轮换和健康监控"""
    
    def __init__(
        self,
        config: ProviderConfig,
        bandwidth_policy: Optional[BandwidthPolicy] = None,
    ) -> None:
        self._config = config
        self._bandwidth_policy = bandwidth_policy or GLOBAL_BANDWIDTH_POLICY
        self._provider_pool: List[Any] = []
        self._current_provider: Optional[Any] = None
        self._fallback_provider: Optional[Any] = None
        self._weak_state = WeakNetworkState()
        self._deferred_tasks: List[Any] = []
        self._provider_health: Dict[str, ProviderHealth] = {}
        self._last_rotation_time: float = 0
        self._min_rotation_interval: float = 5.0  # 最小轮换间隔（秒）

    @property
    def current_provider(self) -> Any:
        """获取当前 provider"""
        if self._current_provider is None:
            raise RuntimeError("ProviderManager has no active provider.")
        return self._current_provider

    @property
    def pool_size(self) -> int:
        """获取 provider 池大小"""
        if not self._provider_pool:
            self.build_provider_pool()
        return len(self._provider_pool)

    @property
    def weak_network_active(self) -> bool:
        """是否处于弱网模式"""
        return self._weak_state.active

    @property
    def weak_state(self) -> WeakNetworkState:
        """获取弱网状态"""
        return self._weak_state

    def build_provider_pool(self) -> None:
        """构建 provider 池"""
        pool = []
        for name in self._config.names:
            provider = self._resolve_provider(name)
            if provider:
                pool.append(provider)
                # 初始化健康状态
                self._provider_health[name] = ProviderHealth(name=name)
        
        self._provider_pool = pool
        self._fallback_provider = pool[0] if pool else None
        logger.info("Provider 池已构建，包含 %d 个 providers", len(pool))

    async def select_best_provider(self) -> ProviderSelectionResult:
        """
        选择最佳 provider
        
        Returns:
            Provider 选择结果
        """
        if not self._provider_pool:
            self.build_provider_pool()
        
        logger.info("开始测速 %d 个 providers...", len(self._provider_pool))
        
        tasks = [self._benchmark_provider(provider) for provider in self._provider_pool]
        results = await asyncio.gather(*tasks)
        
        # 筛选可用的 providers
        available = [
            (res.duration, provider, res)
            for res, provider in zip(results, self._provider_pool)
            if res.success and res.duration is not None
        ]
        
        # 过滤受限的 providers
        filtered = [
            entry
            for entry in available
            if self._get_provider_name(entry[1]) not in self._config.restricted_names
        ]
        
        winning_pool = filtered or available
        
        if not winning_pool:
            logger.warning("没有可用的 providers，使用备用 provider")
            if self._fallback_provider:
                self._current_provider = self._fallback_provider
                return ProviderSelectionResult(self._current_provider, results)
            raise RuntimeError("No available providers after benchmarking.")
        
        # 选择最快的 provider
        winning_entry = min(winning_pool, key=lambda item: item[0])
        self._current_provider = winning_entry[1]
        
        # 更新健康状态
        provider_name = self._get_provider_name(self._current_provider)
        if provider_name in self._provider_health:
            self._provider_health[provider_name].is_healthy = True
            self._provider_health[provider_name].last_check = time.time()
        
        logger.info("已选择最佳 provider: %s (耗时: %.2fs)", provider_name, winning_entry[0])
        
        return ProviderSelectionResult(self._current_provider, results)

    def rotate_provider(self, reason: str) -> None:
        """
        轮换到下一个 provider
        
        Args:
            reason: 轮换原因
        """
        current_time = time.time()
        
        # 防止频繁轮换
        if current_time - self._last_rotation_time < self._min_rotation_interval:
            logger.debug("轮换间隔过短，跳过轮换")
            return
        
        if not self._provider_pool:
            self.build_provider_pool()
        
        if not self._provider_pool:
            raise RuntimeError("No providers available for rotation.")
        
        old_provider = self._current_provider
        old_name = self._get_provider_name(old_provider) if old_provider else "unknown"
        
        # 标记当前 provider 为不健康
        if old_name in self._provider_health:
            health = self._provider_health[old_name]
            health.failure_count += 1
            if health.failure_count >= health.max_failures:
                health.is_healthy = False
                logger.warning("Provider %s 已标记为不健康（失败次数: %d）", old_name, health.failure_count)
        
        # 查找下一个健康的 provider
        if self._current_provider not in self._provider_pool:
            self._current_provider = self._provider_pool[0]
        else:
            idx = self._provider_pool.index(self._current_provider)
            
            # 尝试找到下一个健康的 provider
            for i in range(1, len(self._provider_pool) + 1):
                next_idx = (idx + i) % len(self._provider_pool)
                next_provider = self._provider_pool[next_idx]
                next_name = self._get_provider_name(next_provider)
                
                # 检查健康状态
                if next_name in self._provider_health:
                    if self._provider_health[next_name].is_healthy:
                        self._current_provider = next_provider
                        break
                else:
                    # 未知健康状态，尝试使用
                    self._current_provider = next_provider
                    break
            else:
                # 所有 provider 都不健康，使用下一个
                next_idx = (idx + 1) % len(self._provider_pool)
                self._current_provider = self._provider_pool[next_idx]
        
        new_name = self._get_provider_name(self._current_provider)
        self._last_rotation_time = current_time
        
        logger.info("Provider 已轮换: %s -> %s (原因: %s)", old_name, new_name, reason)

    def get_provider_name(self, provider: Optional[Any] = None) -> str:
        """
        获取 provider 名称
        
        Args:
            provider: Provider 对象，如果为 None 则使用当前 provider
            
        Returns:
            Provider 名称
        """
        target = provider or self._current_provider
        if target is None:
            return "unknown"
        return self._get_provider_name(target)

    def record_bandwidth(self, tokens: int) -> None:
        """
        记录带宽使用
        
        Args:
            tokens: 使用的 token 数量
        """
        name = self.get_provider_name()
        self._bandwidth_policy.record(name, tokens)
        logger.debug("已记录带宽使用: %s tokens (provider: %s)", tokens, name)

    def exceeded_bandwidth(self) -> bool:
        """
        检查是否超过带宽限制
        
        Returns:
            是否超过限制
        """
        name = self.get_provider_name()
        return self._bandwidth_policy.over_limit(name)

    def enable_weak_network(self, reason: str) -> None:
        """
        启用弱网模式
        
        Args:
            reason: 启用原因
        """
        # 切换到稳定的 provider
        stable = self._resolve_provider(self._config.stable_name)
        if stable:
            self._current_provider = stable
            logger.info("已切换到稳定 provider: %s", self._config.stable_name)
        
        self._weak_state = WeakNetworkState(
            active=True,
            reason=reason,
            activated_at=time.time(),
            success_counter=0,
        )
        
        logger.warning("弱网模式已开启：%s", reason)

    def record_success(self) -> None:
        """记录成功的请求"""
        provider_name = self.get_provider_name()
        
        # 更新健康状态
        if provider_name in self._provider_health:
            health = self._provider_health[provider_name]
            health.success_count += 1
            health.failure_count = max(0, health.failure_count - 1)  # 减少失败计数
            
            # 如果连续成功，标记为健康
            if health.success_count >= 2:
                health.is_healthy = True
        
        # 处理弱网模式
        if self._weak_state.active:
            self._weak_state.success_counter += 1
            logger.debug("弱网模式成功计数: %d/%d", 
                        self._weak_state.success_counter, 
                        self._weak_state.required_successes)
            
            if self._weak_state.success_counter >= self._weak_state.required_successes:
                self.disable_weak_network("连续成功请求")
        else:
            self._restore_deferred_on_success()

    def disable_weak_network(self, reason: str) -> None:
        """
        禁用弱网模式
        
        Args:
            reason: 禁用原因
        """
        if not self._weak_state.active:
            return
        
        duration = time.time() - (self._weak_state.activated_at or 0)
        logger.info("弱网模式结束：%s (持续时间: %.1f 秒)", reason, duration)
        
        self._weak_state = WeakNetworkState(active=False)
        self._restore_deferred_on_success()

    def defer_task(self, task: Any) -> None:
        """
        延迟任务，等待网络恢复
        
        Args:
            task: 要延迟的任务
        """
        self._deferred_tasks.append(task)
        logger.info("任务已暂存，等待网络恢复 (待处理: %d)", len(self._deferred_tasks))

    def has_deferred_tasks(self) -> bool:
        """是否有延迟的任务"""
        return bool(self._deferred_tasks)

    def pop_deferred_tasks(self) -> List[Any]:
        """
        获取并清空所有延迟的任务
        
        Returns:
            延迟的任务列表
        """
        pending = list(self._deferred_tasks)
        self._deferred_tasks.clear()
        return pending

    def get_health_status(self) -> Dict[str, ProviderHealth]:
        """
        获取所有 provider 的健康状态
        
        Returns:
            健康状态字典
        """
        return dict(self._provider_health)

    def _restore_deferred_on_success(self) -> None:
        """网络恢复时准备恢复延迟的任务"""
        if self._deferred_tasks and not self._weak_state.active:
            logger.info("网络恢复，准备重新执行 %d 个待办任务", len(self._deferred_tasks))

    def _resolve_provider(self, name: str) -> Optional[Any]:
        """
        解析 provider 名称到 provider 对象
        
        Args:
            name: Provider 名称
            
        Returns:
            Provider 对象，如果不存在则返回 None
        """
        provider = getattr(g4f.Provider, name, None)
        if provider:
            return provider
        
        # 尝试小写
        lower = getattr(g4f.Provider, name.lower(), None)
        if lower:
            logger.debug("Provider.%s 不存在，使用小写版本", name)
            return lower
        
        logger.error("无法解析 provider: %s", name)
        return None

    async def _benchmark_provider(self, provider: Any) -> ProviderTestResult:
        """
        测试 provider 性能
        
        Args:
            provider: 要测试的 provider
            
        Returns:
            测试结果
        """
        provider_name = self._get_provider_name(provider)
        start = time.perf_counter()
        
        try:
            logger.debug("开始测试 provider: %s", provider_name)
            
            resp = await asyncio.wait_for(
                g4f.ChatCompletion.create_async(
                    model=g4f.models.default,
                    provider=provider,
                    messages=[{"role": "user", "content": self._config.benchmark_prompt}],
                    timeout=10,
                ),
                timeout=self._config.benchmark_timeout,
            )
            
            duration = time.perf_counter() - start
            success = isinstance(resp, (str, dict))
            
            if success:
                logger.debug("Provider %s 测试成功 (耗时: %.2fs)", provider_name, duration)
            else:
                logger.debug("Provider %s 测试失败: 响应类型异常", provider_name)
            
            return ProviderTestResult(
                name=provider_name,
                success=success,
                duration=duration if success else None,
                error=None if success else "unexpected response",
            )
            
        except asyncio.TimeoutError:
            logger.debug("Provider %s 测试超时", provider_name)
            return ProviderTestResult(
                name=provider_name,
                success=False,
                duration=None,
                error="timeout",
            )
            
        except Exception as exc:
            logger.debug("Provider %s 测试失败: %s", provider_name, exc)
            return ProviderTestResult(
                name=provider_name,
                success=False,
                duration=None,
                error=str(exc),
            )

    def _get_provider_name(self, provider: Any) -> str:
        """获取 provider 的名称"""
        return getattr(provider, "__name__", str(provider))
