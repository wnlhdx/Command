from __future__ import annotations

import argparse
import asyncio
import contextlib
import logging
import os
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional

from llm import (
    G4FRemoteProvider,
    LLMGenerationError,
    ProviderUnavailableError,
    estimate_tokens,
    set_llm_verbose,
)
from providers_improved import (
    ProviderConfig,
    ProviderManager,
    ProviderSelectionResult,
)
from tts_improved import (
    TTSConfig,
    TTSError,
    flush_tts_queue,
    get_failed_jobs_count,
    play_audio,
    play_audio_stream,
    queue_failed_tts_job,
    set_tts_verbose,
    synthesize_tts,
    synthesize_tts_stream,
)

logger = logging.getLogger(__name__)

# 配置常量
PROVIDER_NAMES = [
    "Yqcloud",
    "TeachAnything",
    "PollinationsAI",
]
RESTRICTED_PROVIDER_NAMES = {"Mintlify"}
STABLE_PROVIDER_NAME = "Yqcloud"
DEFAULT_TTS_VOICE = "zh-CN-XiaoxiaoNeural"
DEFAULT_TTS_RATE = "+0%"
DEFAULT_TTS_VOLUME = "+0%"
DEFAULT_TTS_DIR = Path("tts_output")
WEAK_MODE_CHAR_LIMIT = 300


@dataclass
class AgentTask:
    """异步任务"""
    kind: str
    payload: Dict[str, Any]
    requires_network: bool = True
    retries: int = 0
    max_retries: int = 3


@dataclass
class CLIOptions:
    """CLI 选项"""
    prompt: Optional[str]
    tts_voice: str
    tts_dir: Path
    tts_rate: str
    tts_volume: str
    tts_immediate: bool
    tts_play: bool
    tts_streaming: bool
    verbose: bool


def parse_args() -> CLIOptions:
    """解析命令行参数"""
    parser = argparse.ArgumentParser(
        description="g4f provider 测速与对话工具 - 支持异步 TTS 和弱网模式"
    )
    parser.add_argument("--prompt", help="一次性提问，处理后保持运行等待 TTS 完成")
    parser.add_argument("--tts-voice", default=DEFAULT_TTS_VOICE, help="TTS 语音")
    parser.add_argument("--tts-dir", type=Path, default=DEFAULT_TTS_DIR, help="TTS 输出目录")
    parser.add_argument("--tts-rate", default=DEFAULT_TTS_RATE, help="TTS 语速")
    parser.add_argument("--tts-volume", default=DEFAULT_TTS_VOLUME, help="TTS 音量")
    parser.add_argument(
        "--tts-immediate",
        action="store_true",
        help="配合 --prompt 在输出后立即进行 TTS",
    )
    parser.add_argument(
        "--no-tts-play",
        action="store_true",
        help="生成语音时不要自动播放",
    )
    parser.add_argument(
        "--tts-streaming",
        action="store_true",
        help="启用流式 TTS 生成和播放",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="打印详细调试信息",
    )
    
    args = parser.parse_args()
    
    return CLIOptions(
        prompt=args.prompt,
        tts_voice=args.tts_voice,
        tts_dir=args.tts_dir,
        tts_rate=args.tts_rate,
        tts_volume=args.tts_volume,
        tts_immediate=args.tts_immediate,
        tts_play=not args.no_tts_play,
        tts_streaming=args.tts_streaming,
        verbose=args.verbose,
    )


def build_provider_manager() -> ProviderManager:
    """构建 Provider 管理器"""
    config = ProviderConfig(
        names=PROVIDER_NAMES,
        restricted_names=RESTRICTED_PROVIDER_NAMES,
        stable_name=STABLE_PROVIDER_NAME,
    )
    manager = ProviderManager(config)
    manager.build_provider_pool()
    return manager


def build_tts_config(options: CLIOptions) -> TTSConfig:
    """构建 TTS 配置"""
    return TTSConfig(
        voice=options.tts_voice,
        output_dir=options.tts_dir,
        rate=options.tts_rate,
        volume=options.tts_volume,
        auto_play=options.tts_play,
        streaming_enabled=options.tts_streaming,
    )


def verbose_log(options: CLIOptions, message: str) -> None:
    """详细日志输出"""
    if options.verbose:
        print(f"[verbose] {message}")


def make_chat_task(
    user_text: str,
    *,
    auto_tts: bool = False,
    source: str = "cli",
) -> AgentTask:
    """创建对话任务"""
    return AgentTask(
        kind="chat",
        payload={"user_text": user_text, "source": source, "auto_tts": auto_tts},
        requires_network=True,
        max_retries=3,
    )


def make_tts_task(text: str, *, source: str = "cli") -> AgentTask:
    """创建 TTS 任务"""
    return AgentTask(
        kind="tts",
        payload={"text": text, "source": source},
        requires_network=True,
        max_retries=3,
    )


def plan_user_command(command: str, options: CLIOptions, *, source: str) -> List[AgentTask]:
    """
    解析用户命令并创建任务
    
    支持的命令:
    - tts:文本 - 直接进行 TTS
    - 其他文本 - 作为对话输入
    """
    tasks: List[AgentTask] = []
    
    if command.startswith("tts:"):
        text = command[4:].strip()
        if text:
            tasks.append(make_tts_task(text, source=source))
        else:
            print("[提示] tts: 命令需要提供文本内容")
        return tasks
    
    if command:
        tasks.append(
            make_chat_task(
                command,
                auto_tts=options.tts_immediate and source == "prompt",
                source=source,
            )
        )
    
    return tasks


def resolve_llm_mode() -> str:
    """解析 LLM 模式"""
    return os.getenv("LLM_MODE", "remote").lower()


def create_backend(mode: str, provider_cls: Any) -> G4FRemoteProvider:
    """创建 LLM 后端"""
    if mode == "local":
        logger.info("本地模式尚未实现，自动回退到远程模型")
        mode = "remote"
    elif mode == "edge":
        logger.info("边缘模式尚未实现，自动回退到远程模型")
        mode = "remote"
    
    return G4FRemoteProvider(provider_cls)


async def generate_with_failover(
    manager: ProviderManager,
    messages: List[dict],
    mode: str,
) -> str:
    """
    带故障转移的生成
    
    尝试使用当前 provider 生成，失败时自动轮换
    """
    attempts = 0
    max_attempts = max(1, manager.pool_size)
    last_error: Optional[Exception] = None
    
    while attempts < max_attempts:
        backend = create_backend(mode, manager.current_provider)
        
        try:
            reply = await backend.generate(messages)
            manager.record_success()
            return reply
            
        except ProviderUnavailableError as exc:
            last_error = exc
            manager.enable_weak_network(str(exc))
            manager.rotate_provider("provider failure")
            attempts += 1
            continue
            
        except LLMGenerationError as exc:
            manager.enable_weak_network(str(exc))
            raise
    
    raise ProviderUnavailableError(
        str(last_error) if last_error else "所有 provider 均不可用"
    )


def apply_weak_mode_policy(reply: str, weak_mode: bool) -> str:
    """应用弱网模式策略"""
    if not weak_mode:
        return reply
    return reply[:WEAK_MODE_CHAR_LIMIT]


def display_benchmark(result: ProviderSelectionResult) -> None:
    """显示测速结果"""
    print("正在测速 provider...\n")
    
    for test in result.tests:
        if test.success and test.duration is not None:
            print(f"  {test.name}: {test.duration:.2f}s")
        else:
            detail = f" (失败: {test.error})" if test.error else " (失败)"
            print(f"  {test.name}{detail}")
    
    print()
    best_name = getattr(result.best_provider, "__name__", "unknown")
    print(f"[OK] 已选择最快: {best_name}\n")


async def handle_interactive_mode(
    manager: ProviderManager,
    options: CLIOptions,
    messages: List[dict],
    mode: str,
    tts_config: TTSConfig,
    task_queue: "asyncio.Queue[AgentTask]",
    stop_event: asyncio.Event,
) -> None:
    """处理交互模式"""
    print("助理已就绪（输入 exit 退出）\n")
    print("支持的命令:")
    print("  - 直接输入文本进行对话")
    print("  - tts:文本 - 将文本转换为语音")
    print("  - exit - 退出程序\n")
    
    while not stop_event.is_set():
        try:
            # 使用 asyncio 友好的输入方式
            user = await asyncio.get_event_loop().run_in_executor(
                None, lambda: input("您: ").strip()
            )
        except EOFError:
            print("\n[提示] 检测到输入结束，程序退出")
            break
        except KeyboardInterrupt:
            print("\n[提示] 检测到中断信号，程序退出")
            break
        
        if not user:
            print("[提示] 输入为空，请重新输入")
            continue
        
        if user.lower() == "exit":
            print("[提示] 正在退出...")
            break
        
        tasks = plan_user_command(user, options, source="cli")
        for task in tasks:
            await task_queue.put(task)
    
    stop_event.set()


async def process_chat_task(
    task: AgentTask,
    queue: "asyncio.Queue[AgentTask]",
    manager: ProviderManager,
    options: CLIOptions,
    messages: List[dict],
    mode: str,
    tts_config: TTSConfig,
) -> None:
    """处理对话任务"""
    user_text = task.payload.get("user_text", "")
    messages.append({"role": "user", "content": user_text})
    
    verbose_log(options, f"开始处理对话任务（来源: {task.payload.get('source')}）")
    print(f"[Provider] 当前使用: {manager.get_provider_name()}")
    
    try:
        reply = await generate_with_failover(manager, messages, mode)
    except Exception:
        messages.pop()  # 回滚用户消息，等待重试
        raise
    
    reply = apply_weak_mode_policy(reply, manager.weak_network_active)
    messages.append({"role": "assistant", "content": reply})
    
    print(f"AI: {reply}")
    
    # 记录带宽使用
    tokens = estimate_tokens(messages, reply)
    manager.record_bandwidth(tokens)
    
    if manager.exceeded_bandwidth():
        manager.rotate_provider("bandwidth limit reached")
        print("[提示] 已自动切换 Provider 以避免带宽限制")
    
    if manager.weak_network_active:
        print("[弱网] 当前处于弱网模式，已限制输出长度并关闭 TTS")
    
    # 自动 TTS
    if task.payload.get("auto_tts") and not manager.weak_network_active:
        await queue.put(make_tts_task(reply, source="auto"))
    
    # 恢复延迟的任务
    await restore_deferred_tasks(manager, queue, options)


async def process_tts_task(
    task: AgentTask,
    queue: "asyncio.Queue[AgentTask]",
    manager: ProviderManager,
    options: CLIOptions,
    tts_config: TTSConfig,
) -> None:
    """处理 TTS 任务"""
    text = task.payload.get("text", "")
    
    verbose_log(options, f"开始处理 TTS 任务（长度 {len(text)} 字符）")
    
    try:
        if tts_config.streaming_enabled:
            audio_stream = synthesize_tts_stream(text, tts_config)
            await play_audio_stream(audio_stream, tts_config)
            print("[TTS] 流式语音播放完成")
        else:
            output = await synthesize_tts(text, tts_config)
            print(f"[TTS] 已生成语音: {output}")
            if tts_config.auto_play:
                play_audio(output)
                
    except TTSError as exc:
        print(f"[TTS 错误] {exc}")
        queue_failed_tts_job(text, tts_config, attempt=task.retries)
        return
    
    # 恢复延迟的任务
    await restore_deferred_tasks(manager, queue, options)


async def restore_deferred_tasks(
    manager: ProviderManager,
    queue: "asyncio.Queue[AgentTask]",
    options: CLIOptions,
) -> None:
    """恢复延迟的任务"""
    if manager.weak_network_active:
        return
    
    if manager.has_deferred_tasks():
        pending = manager.pop_deferred_tasks()
        verbose_log(options, f"网络恢复，重新排入 {len(pending)} 个任务")
        for pending_task in pending:
            await queue.put(pending_task)
    
    # 处理失败的 TTS 任务
    await flush_tts_queue()


async def process_task(
    task: AgentTask,
    queue: "asyncio.Queue[AgentTask]",
    manager: ProviderManager,
    options: CLIOptions,
    messages: List[dict],
    mode: str,
    tts_config: TTSConfig,
) -> None:
    """处理单个任务"""
    # 弱网模式下延迟 TTS 任务
    if task.kind == "tts" and manager.weak_network_active:
        manager.defer_task(task)
        print("[弱网] TTS 任务已暂存，稍后自动继续")
        return
    
    try:
        if task.kind == "chat":
            await process_chat_task(
                task, queue, manager, options, messages, mode, tts_config
            )
        elif task.kind == "tts":
            await process_tts_task(task, queue, manager, options, tts_config)
        else:
            verbose_log(options, f"未知任务类型: {task.kind}")
            
    except (LLMGenerationError, ProviderUnavailableError) as exc:
        print(f"[错误] {exc}")
        
        if task.retries + 1 < task.max_retries:
            task.retries += 1
            verbose_log(options, f"任务重试 ({task.retries}/{task.max_retries})")
            await queue.put(task)
        else:
            manager.defer_task(task)
            print("[提示] 任务已达到最大重试次数，已暂存等待网络恢复")
            
    except Exception as exc:
        print(f"[错误] 未处理异常: {exc}")
        logger.exception("未处理异常")


async def task_worker(
    queue: "asyncio.Queue[AgentTask]",
    manager: ProviderManager,
    options: CLIOptions,
    messages: List[dict],
    mode: str,
    tts_config: TTSConfig,
    stop_event: asyncio.Event,
) -> None:
    """任务工作器"""
    while True:
        if stop_event.is_set() and queue.empty():
            # 检查是否还有待处理的 TTS 任务
            if get_failed_jobs_count() == 0:
                break
        
        try:
            task = await asyncio.wait_for(queue.get(), timeout=0.5)
        except asyncio.TimeoutError:
            continue
        
        try:
            await process_task(
                task, queue, manager, options, messages, mode, tts_config
            )
        finally:
            queue.task_done()


async def run_cli() -> None:
    """运行 CLI"""
    # 配置日志
    logging.basicConfig(
        level=logging.INFO,
        format="[%(levelname)s] %(message)s"
    )
    
    options = parse_args()
    
    # 设置详细模式
    if options.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    set_llm_verbose(options.verbose)
    set_tts_verbose(options.verbose)
    
    # 构建管理器
    manager = build_provider_manager()
    
    # 选择最佳 provider
    selection = await manager.select_best_provider()
    display_benchmark(selection)
    
    mode = resolve_llm_mode()
    tts_config = build_tts_config(options)
    
    messages: List[dict] = []
    task_queue: "asyncio.Queue[AgentTask]" = asyncio.Queue()
    stop_event = asyncio.Event()
    
    # 启动任务工作器
    worker = asyncio.create_task(
        task_worker(
            task_queue, manager, options, messages, mode, tts_config, stop_event
        )
    )

    try:
        if options.prompt:
            # 单次 prompt 模式
            tasks = plan_user_command(options.prompt, options, source="prompt")
            for task in tasks:
                await task_queue.put(task)
            
            # 等待任务完成
            await task_queue.join()
            
            # 如果有 TTS 任务，等待一段时间让它完成
            if options.tts_immediate:
                print("[提示] 等待 TTS 任务完成...")
                await asyncio.sleep(2)
            
            stop_event.set()
        else:
            # 交互模式
            await handle_interactive_mode(
                manager, options, messages, mode, tts_config, task_queue, stop_event
            )
            await task_queue.join()
            
    except KeyboardInterrupt:
        print("\n[提示] 检测到中断信号，正在退出...")
        
    finally:
        stop_event.set()
        worker.cancel()
        
        with contextlib.suppress(asyncio.CancelledError):
            await worker
        
        # 显示状态
        if manager.has_deferred_tasks():
            print(f"[提示] 有 {len(manager.pop_deferred_tasks())} 个任务未完成")
        
        if get_failed_jobs_count() > 0:
            print(f"[提示] 有 {get_failed_jobs_count()} 个 TTS 任务未完成")
        
        print("[提示] 程序已退出")


def main() -> None:
    """主入口"""
    try:
        asyncio.run(run_cli())
    except KeyboardInterrupt:
        print("\n[提示] 程序已退出")
        sys.exit(0)


if __name__ == "__main__":
    main()
