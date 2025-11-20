from __future__ import annotations

import argparse
import asyncio
import contextlib
import logging
import os
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
from providers import (
    ProviderConfig,
    ProviderManager,
    ProviderSelectionResult,
    ProviderTestResult,
)
from tts import (
    TTSConfig,
    TTSError,
    flush_tts_queue,
    play_audio,
    play_audio_stream,
    queue_failed_tts_job,
    set_tts_verbose,
    synthesize_tts,
    synthesize_tts_stream,
)

logger = logging.getLogger(__name__)

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
    kind: str
    payload: Dict[str, Any]
    requires_network: bool = True
    retries: int = 0
    max_retries: int = 3


@dataclass
class CLIOptions:
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
    parser = argparse.ArgumentParser(description="g4f provider 测速与对话工具")
    parser.add_argument("--prompt", help="一次性提问，结束后退出。")
    parser.add_argument("--tts-voice", default=DEFAULT_TTS_VOICE)
    parser.add_argument("--tts-dir", type=Path, default=DEFAULT_TTS_DIR)
    parser.add_argument("--tts-rate", default=DEFAULT_TTS_RATE)
    parser.add_argument("--tts-volume", default=DEFAULT_TTS_VOLUME)
    parser.add_argument(
        "--tts-immediate",
        action="store_true",
        help="配合 --prompt 在输出后立即进行 TTS。",
    )
    parser.add_argument(
        "--no-tts-play",
        action="store_true",
        help="生成语音时不要自动播放。",
    )
    parser.add_argument(
        "--tts-streaming",
        action="store_true",
        help="启用流式TTS生成和播放。",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="打印底层调试信息以便学习。",
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
    config = ProviderConfig(
        names=PROVIDER_NAMES,
        restricted_names=RESTRICTED_PROVIDER_NAMES,
        stable_name=STABLE_PROVIDER_NAME,
    )
    manager = ProviderManager(config)
    manager.build_provider_pool()
    return manager


def build_tts_config(options: CLIOptions) -> TTSConfig:
    return TTSConfig(
        voice=options.tts_voice,
        output_dir=options.tts_dir,
        rate=options.tts_rate,
        volume=options.tts_volume,
        auto_play=options.tts_play,
        streaming_enabled=options.tts_streaming,
    )


def verbose_log(options: CLIOptions, message: str) -> None:
    if options.verbose:
        print(f"[verbose] {message}")


def make_chat_task(
    user_text: str,
    *,
    auto_tts: bool = False,
    source: str = "cli",
) -> AgentTask:
    return AgentTask(
        kind="chat",
        payload={"user_text": user_text, "source": source, "auto_tts": auto_tts},
        requires_network=True,
        max_retries=3,
    )


def make_tts_task(text: str, *, source: str = "cli") -> AgentTask:
    return AgentTask(
        kind="tts",
        payload={"text": text, "source": source},
        requires_network=True,
        max_retries=3,
    )


def plan_user_command(command: str, options: CLIOptions, *, source: str) -> List[AgentTask]:
    tasks: List[AgentTask] = []
    if command.startswith("tts:"):
        text = command[4:].strip()
        if text:
            tasks.append(make_tts_task(text, source=source))
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
    return os.getenv("LLM_MODE", "remote").lower()


def create_backend(mode: str, provider_cls: Any) -> G4FRemoteProvider:
    if mode == "local":
        logger.info("本地模式尚未实现，自动回退到远程模型。")
        mode = "remote"
    elif mode == "edge":
        logger.info("边缘模式尚未实现，自动回退到远程模型。")
        mode = "remote"
    return G4FRemoteProvider(provider_cls)


async def generate_with_failover(
    manager: ProviderManager,
    messages: List[dict],
    mode: str,
) -> str:
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
    raise ProviderUnavailableError(str(last_error) if last_error else "所有 provider 均不可用")


def apply_weak_mode_policy(reply: str, weak_mode: bool) -> str:
    if not weak_mode:
        return reply
    return reply[:WEAK_MODE_CHAR_LIMIT]


def display_benchmark(result: ProviderSelectionResult) -> None:
    print("正在测速 provider...\n")
    for test in result.tests:
        if test.success and test.duration is not None:
            print(f"{test.name} 用时 {test.duration:.2f}s")
        else:
            detail = f" 测试失败：{test.error}" if test.error else " 测试失败"
            print(f"{test.name}{detail}")
    print()
    best_name = getattr(result.best_provider, "__name__", "unknown")
    print(f"[OK] 已选择最快：{best_name}\n")


async def handle_interactive_mode(
    manager: ProviderManager,
    options: CLIOptions,
    messages: List[dict],
    mode: str,
    tts_config: TTSConfig,
    task_queue: "asyncio.Queue[AgentTask]",
    stop_event: asyncio.Event,
) -> None:
    print("助理已就绪（输入 exit 退出）\n")
    while not stop_event.is_set():
        try:
            user = input("您: ").strip()
        except EOFError:
            print("\n[提示] 检测到输入结束，程序退出。")
            break
        if not user:
            print("[提示] 输入为空，请重新输入。")
            continue
        if user.lower() == "exit":
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
    user_text = task.payload.get("user_text", "")
    messages.append({"role": "user", "content": user_text})
    verbose_log(options, f"开始处理对话任务（来源：{task.payload.get('source')}）")
    print(f"[Provider] 当前使用: {manager.get_provider_name()}")
    try:
        reply = await generate_with_failover(manager, messages, mode)
    except Exception:
        messages.pop()  # 回滚用户消息，等待重试
        raise
    reply = apply_weak_mode_policy(reply, manager.weak_network_active)
    messages.append({"role": "assistant", "content": reply})
    print("AI:", reply)
    tokens = estimate_tokens(messages, reply)
    manager.record_bandwidth(tokens)
    if manager.exceeded_bandwidth():
        manager.rotate_provider("bandwidth limit reached")
        print("[提示] 已自动切换 Provider 以避免带宽限制。")
    if manager.weak_network_active:
        print("[弱网] 当前处于弱网模式，已限制输出长度并关闭 TTS。")
    if task.payload.get("auto_tts") and not manager.weak_network_active:
        await queue.put(make_tts_task(reply, source="auto"))
    await restore_deferred_tasks(manager, queue, options)


async def process_tts_task(
    task: AgentTask,
    queue: "asyncio.Queue[AgentTask]",
    manager: ProviderManager,
    options: CLIOptions,
    tts_config: TTSConfig,
) -> None:
    text = task.payload.get("text", "")
    verbose_log(options, f"开始处理 TTS 任务（长度 {len(text)} 字符）")
    try:
        if tts_config.streaming_enabled:
            audio_stream = synthesize_tts_stream(text, tts_config)
            await play_audio_stream(audio_stream, tts_config)
            print(f"[TTS] 流式语音播放完成")
        else:
            output = await synthesize_tts(text, tts_config)
            print(f"[TTS] 已生成语音：{output}")
            if tts_config.auto_play:
                play_audio(output)
    except TTSError as exc:
        print(f"[TTS 错误] {exc}")
        queue_failed_tts_job(text, tts_config, attempt=task.retries)
        return
    await restore_deferred_tasks(manager, queue, options)


async def restore_deferred_tasks(
    manager: ProviderManager,
    queue: "asyncio.Queue[AgentTask]",
    options: CLIOptions,
) -> None:
    if manager.weak_network_active:
        return
    if manager.has_deferred_tasks():
        pending = manager.pop_deferred_tasks()
        verbose_log(options, f"网络恢复，重新排入 {len(pending)} 个任务。")
        for pending_task in pending:
            await queue.put(pending_task)
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
    if task.kind == "tts" and manager.weak_network_active:
        manager.defer_task(task)
        print("[弱网] 当前任务已暂存，稍后自动继续。")
        return
    try:
        if task.kind == "chat":
            await process_chat_task(task, queue, manager, options, messages, mode, tts_config)
        elif task.kind == "tts":
            await process_tts_task(task, queue, manager, options, tts_config)
        else:
            verbose_log(options, f"未知任务类型：{task.kind}")
    except (LLMGenerationError, ProviderUnavailableError) as exc:
        print(f"[错误] {exc}")
        if task.retries + 1 < task.max_retries:
            task.retries += 1
            await queue.put(task)
        else:
            manager.defer_task(task)
    except Exception as exc:  # noqa: BLE001
        print(f"[错误] 未处理异常：{exc}")


async def task_worker(
    queue: "asyncio.Queue[AgentTask]",
    manager: ProviderManager,
    options: CLIOptions,
    messages: List[dict],
    mode: str,
    tts_config: TTSConfig,
    stop_event: asyncio.Event,
) -> None:
    while True:
        if stop_event.is_set() and queue.empty():
            break
        try:
            task = await asyncio.wait_for(queue.get(), timeout=0.5)
        except asyncio.TimeoutError:
            continue
        try:
            await process_task(task, queue, manager, options, messages, mode, tts_config)
        finally:
            queue.task_done()


async def run_cli() -> None:
    logging.basicConfig(level=logging.INFO, format="[%(levelname)s] %(message)s")
    options = parse_args()
    set_llm_verbose(options.verbose)
    set_tts_verbose(options.verbose)
    manager = build_provider_manager()
    selection = await manager.select_best_provider()
    display_benchmark(selection)
    mode = resolve_llm_mode()
    tts_config = build_tts_config(options)
    messages: List[dict] = []
    task_queue: "asyncio.Queue[AgentTask]" = asyncio.Queue()
    stop_event = asyncio.Event()
    worker = asyncio.create_task(
        task_worker(task_queue, manager, options, messages, mode, tts_config, stop_event)
    )

    try:
        if options.prompt:
            tasks = plan_user_command(options.prompt, options, source="prompt")
            for task in tasks:
                await task_queue.put(task)
            await task_queue.join()
            stop_event.set()
        else:
            await handle_interactive_mode(
                manager, options, messages, mode, tts_config, task_queue, stop_event
            )
            await task_queue.join()
    finally:
        stop_event.set()
        worker.cancel()
        with contextlib.suppress(asyncio.CancelledError):
            await worker


if __name__ == "__main__":
    asyncio.run(run_cli())