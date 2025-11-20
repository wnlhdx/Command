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
    name: str
    success: bool
    duration: Optional[float]
    error: Optional[str] = None


@dataclass
class ProviderSelectionResult:
    best_provider: Any
    tests: List[ProviderTestResult]


@dataclass
class ProviderConfig:
    names: Sequence[str]
    restricted_names: Sequence[str]
    stable_name: str
    benchmark_prompt: str = "hi"


@dataclass
class WeakNetworkState:
    active: bool = False
    reason: Optional[str] = None
    activated_at: Optional[float] = None
    success_counter: int = 0


class ProviderManager:
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

    @property
    def current_provider(self) -> Any:
        if self._current_provider is None:
            raise RuntimeError("ProviderManager has no active provider.")
        return self._current_provider

    @property
    def pool_size(self) -> int:
        if not self._provider_pool:
            self.build_provider_pool()
        return len(self._provider_pool)

    @property
    def weak_network_active(self) -> bool:
        return self._weak_state.active

    @property
    def weak_state(self) -> WeakNetworkState:
        return self._weak_state

    def build_provider_pool(self) -> None:
        pool = []
        for name in self._config.names:
            provider = self._resolve_provider(name)
            if provider:
                pool.append(provider)
        self._provider_pool = pool
        self._fallback_provider = pool[0] if pool else None
        logger.debug("Provider pool contains %s providers.", len(pool))

    async def select_best_provider(self) -> ProviderSelectionResult:
        if not self._provider_pool:
            self.build_provider_pool()
        tasks = [self._benchmark_provider(provider) for provider in self._provider_pool]
        results = await asyncio.gather(*tasks)
        available = [
            (res.duration, provider)
            for res, provider in zip(results, self._provider_pool)
            if res.success and res.duration is not None
        ]
        filtered = [
            entry
            for entry in available
            if self._get_provider_name(entry[1]) not in self._config.restricted_names
        ]
        winning_pool = filtered or available
        if not winning_pool:
            if self._fallback_provider:
                self._current_provider = self._fallback_provider
                return ProviderSelectionResult(self._current_provider, results)
            raise RuntimeError("No available providers after benchmarking.")
        winning_entry = min(winning_pool, key=lambda item: item[0])
        self._current_provider = winning_entry[1]
        return ProviderSelectionResult(self._current_provider, results)

    def rotate_provider(self, reason: str) -> None:
        if not self._provider_pool:
            self.build_provider_pool()
        if not self._provider_pool:
            raise RuntimeError("No providers available for rotation.")
        if self._current_provider not in self._provider_pool:
            self._current_provider = self._provider_pool[0]
            return
        idx = self._provider_pool.index(self._current_provider)
        next_idx = (idx + 1) % len(self._provider_pool)
        self._current_provider = self._provider_pool[next_idx]
        logger.info("Provider rotated due to %s: %s", reason, self._get_provider_name(self._current_provider))

    def get_provider_name(self, provider: Optional[Any] = None) -> str:
        target = provider or self._current_provider
        if target is None:
            return "unknown"
        return self._get_provider_name(target)

    def record_bandwidth(self, tokens: int) -> None:
        name = self.get_provider_name()
        self._bandwidth_policy.record(name, tokens)

    def exceeded_bandwidth(self) -> bool:
        name = self.get_provider_name()
        return self._bandwidth_policy.over_limit(name)

    def enable_weak_network(self, reason: str) -> None:
        stable = self._resolve_provider(self._config.stable_name)
        if stable:
            self._current_provider = stable
        self._weak_state = WeakNetworkState(
            active=True,
            reason=reason,
            activated_at=time.time(),
            success_counter=0,
        )
        logger.warning("弱网模式已开启：%s", reason)

    def record_success(self) -> None:
        if self._weak_state.active:
            self._weak_state.success_counter += 1
            if self._weak_state.success_counter >= 2:
                self.disable_weak_network("连续成功请求")
        else:
            self._restore_deferred_on_success()

    def disable_weak_network(self, reason: str) -> None:
        if not self._weak_state.active:
            return
        logger.info("弱网模式结束：%s", reason)
        self._weak_state = WeakNetworkState(active=False)
        self._restore_deferred_on_success()

    def defer_task(self, task: Any) -> None:
        """Store a task until the network recovers."""
        self._deferred_tasks.append(task)
        logger.info("任务已暂存，等待网络恢复。 (pending=%s)", len(self._deferred_tasks))

    def has_deferred_tasks(self) -> bool:
        return bool(self._deferred_tasks)

    def pop_deferred_tasks(self) -> List[Any]:
        pending = list(self._deferred_tasks)
        self._deferred_tasks.clear()
        return pending

    def _restore_deferred_on_success(self) -> None:
        if self._deferred_tasks and not self._weak_state.active:
            logger.info("网络恢复，准备重新执行 %s 个待办任务。", len(self._deferred_tasks))

    def _resolve_provider(self, name: str) -> Optional[Any]:
        provider = getattr(g4f.Provider, name, None)
        if provider:
            return provider
        lower = getattr(g4f.Provider, name.lower(), None)
        if lower:
            logger.info("Provider.%s 不存在，回退到小写。", name)
            return lower
        logger.error("无法解析 provider: %s", name)
        return None

    async def _benchmark_provider(self, provider: Any) -> ProviderTestResult:
        start = time.perf_counter()
        try:
            resp = await asyncio.wait_for(
                g4f.ChatCompletion.create_async(
                    model=g4f.models.default,
                    provider=provider,
                    messages=[{"role": "user", "content": self._config.benchmark_prompt}],
                    timeout=10,
                ),
                timeout=15,
            )
            duration = time.perf_counter() - start
            success = isinstance(resp, (str, dict))
            return ProviderTestResult(
                name=self._get_provider_name(provider),
                success=success,
                duration=duration if success else None,
                error=None if success else "unexpected response",
            )
        except Exception as exc:  # noqa: BLE001
            logger.debug("Benchmark failed for %s: %s", self._get_provider_name(provider), exc)
            return ProviderTestResult(
                name=self._get_provider_name(provider),
                success=False,
                duration=None,
                error=str(exc),
            )

    def _get_provider_name(self, provider: Any) -> str:
        return getattr(provider, "__name__", str(provider))

