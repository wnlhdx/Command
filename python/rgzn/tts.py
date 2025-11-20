from __future__ import annotations

import asyncio
import io
import logging
import os
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any, AsyncIterator, List, Optional, Tuple

try:
    import edge_tts
except ImportError:  # pragma: no cover
    edge_tts = None

logger = logging.getLogger(__name__)
_VERBOSE = False
_FAILED_JOBS: List[Tuple[str, "TTSConfig", int]] = []


def set_tts_verbose(flag: bool) -> None:
    global _VERBOSE
    _VERBOSE = flag
    logger.setLevel(logging.DEBUG if flag else logging.INFO)


def _vlog(message: str, *args: object) -> None:
    if _VERBOSE:
        logger.debug(message, *args)


class TTSError(Exception):
    """Raised when TTS synthesis fails."""


@dataclass
class TTSConfig:
    voice: str
    output_dir: Path
    rate: str
    volume: str
    auto_play: bool = True
    max_attempts: int = 3
    streaming_enabled: bool = False  # 是否启用流式TTS


async def synthesize_tts(text: str, config: TTSConfig) -> Path:
    if edge_tts is None:
        raise TTSError("edge-tts 未安装，无法使用语音合成功能。")
    sanitized = text.strip()
    if not sanitized:
        raise TTSError("无法生成空文本的语音。")

    config.output_dir.mkdir(parents=True, exist_ok=True)
    last_error: Optional[Exception] = None
    for attempt in range(1, config.max_attempts + 1):
        filename = f"tts_{int(time.time() * 1000)}.mp3"
        output_path = config.output_dir / filename
        try:
            communicator = edge_tts.Communicate(
                sanitized,
                voice=config.voice,
                rate=config.rate,
                volume=config.volume,
            )
            await communicator.save(str(output_path))
            if output_path.exists() and output_path.stat().st_size > 0:
                _vlog("TTS 合成成功: %s", output_path)
                return output_path
            last_error = TTSError("生成的音频文件为空或不存在。")
            logger.warning("TTS 输出为空，进行第 %s 次重试。", attempt)
        except Exception as exc:  # noqa: BLE001
            last_error = exc
            logger.warning("TTS 失败（第 %s 次尝试）：%s", attempt, exc)
    raise TTSError(str(last_error)) from last_error


def play_audio(path: Path) -> None:
    try:
        if os.name == "nt":
            os.startfile(path)  # type: ignore[attr-defined]
        elif sys_platform() == "darwin":
            subprocess.Popen(["afplay", str(path)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        else:
            subprocess.Popen(["xdg-open", str(path)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except Exception as exc:  # noqa: BLE001
        logger.warning("无法自动播放音频：%s", exc)


async def synthesize_tts_stream(text: str, config: TTSConfig) -> AsyncIterator[bytes]:
    """流式生成TTS音频数据"""
    if edge_tts is None:
        raise TTSError("edge-tts 未安装，无法使用语音合成功能。")
    sanitized = text.strip()
    if not sanitized:
        raise TTSError("无法生成空文本的语音。")

    logger.info("开始流式TTS合成")
    try:
        # 构建edge-tts参数
        tts_args = {
            "text": sanitized,
            "voice": config.voice,
            # volume需要是百分比格式的字符串，如"+0%"表示正常音量
            "volume": f"+{int((float(config.volume) - 1.0) * 100)}%" if float(config.volume) > 1.0 else f"-{int((1.0 - float(config.volume)) * 100)}%" if float(config.volume) < 1.0 else "+0%",
        }
        
        # 只在rate不是默认值1.0时添加rate参数，避免参数格式问题
        if float(config.rate) != 1.0:
            # 使用edge-tts正确的rate格式，如"+50%"或"-20%"
            rate_percent = int((float(config.rate) - 1.0) * 100)
            tts_args["rate"] = f"{rate_percent:+d}%"
        
        communicator = edge_tts.Communicate(**tts_args)
        
        async for chunk in communicator.stream():
            if chunk["type"] == "audio" and chunk["data"]:
                yield chunk["data"]
                await asyncio.sleep(0)  # 让出控制权，避免阻塞
    except Exception as exc:
        logger.error("流式TTS合成失败：%s", exc)
        raise TTSError(f"流式TTS合成失败：{exc}") from exc


async def play_audio_stream(audio_stream: AsyncIterator[bytes], config: TTSConfig) -> None:
    """实时播放流式音频数据"""
    try:
        # 尝试导入pygame用于实时音频播放
        try:
            import pygame
            pygame.mixer.init(frequency=24000, size=-16, channels=1, buffer=512)
            audio_queue = asyncio.Queue()
            playing = True

            async def audio_consumer():
                """音频消费者协程"""
                nonlocal playing
                buffer = bytearray()
                while playing:
                    try:
                        # 尝试获取音频数据，如果没有数据则等待
                        chunk = await asyncio.wait_for(audio_queue.get(), timeout=0.1)
                        buffer.extend(chunk)
                        audio_queue.task_done()
                        
                        # 当缓冲区足够大时播放音频
                        if len(buffer) >= 1024:
                            sound = pygame.mixer.Sound(buffer)
                            sound.play()
                            # 清空缓冲区，但保留一部分以保证连续性
                            buffer = buffer[int(len(buffer) * 0.7):]
                            await asyncio.sleep(0.1)  # 短暂休眠，避免CPU占用过高
                    except asyncio.TimeoutError:
                        # 超时，继续循环
                        if not playing and audio_queue.empty():
                            break

            # 启动音频消费者协程
            consumer_task = asyncio.create_task(audio_consumer())

            # 将音频流数据放入队列
            try:
                async for chunk in audio_stream:
                    await audio_queue.put(chunk)
            finally:
                # 结束播放
                playing = False
                await audio_queue.join()
                await consumer_task
                pygame.mixer.quit()

        except ImportError:
            # 如果pygame不可用，回退到保存文件并播放
            logger.warning("pygame不可用，回退到非流式播放")
            buffer = bytearray()
            async for chunk in audio_stream:
                buffer.extend(chunk)
            
            # 保存到临时文件
            temp_file = config.output_dir / f"temp_stream_{int(time.time() * 1000)}.mp3"
            with open(temp_file, "wb") as f:
                f.write(buffer)
            
            # 播放保存的文件
            play_audio(temp_file)
            
    except Exception as exc:
        logger.error("流式音频播放失败：%s", exc)
        raise TTSError(f"流式音频播放失败：{exc}") from exc


def sys_platform() -> str:
    return os.getenv("OSTYPE", "").lower() or os.name


def queue_failed_tts_job(text: str, config: TTSConfig, *, attempt: int = 0) -> None:
    _FAILED_JOBS.append((text, config, attempt))
    _vlog("TTS 任务已入队待稍后重试（总计 %s）", len(_FAILED_JOBS))


async def flush_tts_queue() -> None:
    if not _FAILED_JOBS:
        return
    pending = list(_FAILED_JOBS)
    _FAILED_JOBS.clear()
    _vlog("尝试重放 %s 个待办 TTS 任务。", len(pending))
    for text, cfg, attempt in pending:
        try:
            await synthesize_tts(text, cfg)
        except TTSError as exc:
            logger.warning("延迟 TTS 仍失败：%s", exc)
            if attempt + 1 < cfg.max_attempts:
                queue_failed_tts_job(text, cfg, attempt=attempt + 1)

