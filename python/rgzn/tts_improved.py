from __future__ import annotations

import asyncio
import logging
import os
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import AsyncIterator, List, Optional, Tuple

try:
    import edge_tts
except ImportError:  # pragma: no cover
    edge_tts = None

logger = logging.getLogger(__name__)
_VERBOSE = False
_FAILED_JOBS: List[Tuple[str, "TTSConfig", int]] = []


def set_tts_verbose(flag: bool) -> None:
    """设置 TTS 详细日志模式"""
    global _VERBOSE
    _VERBOSE = flag
    logger.setLevel(logging.DEBUG if flag else logging.INFO)


def _vlog(message: str, *args: object) -> None:
    """详细日志输出"""
    if _VERBOSE:
        logger.debug(message, *args)


class TTSError(Exception):
    """TTS 合成失败时抛出的异常"""


@dataclass
class TTSConfig:
    """TTS 配置"""
    voice: str
    output_dir: Path
    rate: str = "+0%"
    volume: str = "+0%"
    auto_play: bool = True
    max_attempts: int = 3
    streaming_enabled: bool = False


async def synthesize_tts(text: str, config: TTSConfig) -> Path:
    """
    异步合成 TTS 语音文件
    
    Args:
        text: 要合成的文本
        config: TTS 配置
        
    Returns:
        生成的音频文件路径
        
    Raises:
        TTSError: 合成失败时抛出
    """
    if edge_tts is None:
        raise TTSError("edge-tts 未安装，无法使用语音合成功能。请运行: pip install edge-tts")
    
    sanitized = text.strip()
    if not sanitized:
        raise TTSError("无法生成空文本的语音。")

    config.output_dir.mkdir(parents=True, exist_ok=True)
    last_error: Optional[Exception] = None
    
    for attempt in range(1, config.max_attempts + 1):
        filename = f"tts_{int(time.time() * 1000)}.mp3"
        output_path = config.output_dir / filename
        
        try:
            _vlog("TTS 合成尝试 %d/%d: %s", attempt, config.max_attempts, output_path)
            
            communicator = edge_tts.Communicate(
                sanitized,
                voice=config.voice,
                rate=config.rate,
                volume=config.volume,
            )
            
            await communicator.save(str(output_path))
            
            if output_path.exists() and output_path.stat().st_size > 0:
                _vlog("TTS 合成成功: %s (大小: %d 字节)", output_path, output_path.stat().st_size)
                return output_path
            
            last_error = TTSError("生成的音频文件为空或不存在。")
            logger.warning("TTS 输出为空，进行第 %d 次重试。", attempt)
            
        except Exception as exc:
            last_error = exc
            logger.warning("TTS 失败（第 %d/%d 次尝试）：%s", attempt, config.max_attempts, exc)
            
            if attempt < config.max_attempts:
                await asyncio.sleep(1)  # 重试前等待
    
    raise TTSError(f"TTS 合成失败（已尝试 {config.max_attempts} 次）: {last_error}") from last_error


def play_audio(path: Path) -> None:
    """
    播放音频文件
    
    Args:
        path: 音频文件路径
    """
    try:
        _vlog("播放音频: %s", path)
        
        if os.name == "nt":  # Windows
            os.startfile(path)  # type: ignore[attr-defined]
        elif _sys_platform() == "darwin":  # macOS
            subprocess.Popen(
                ["afplay", str(path)],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )
        else:  # Linux
            subprocess.Popen(
                ["xdg-open", str(path)],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )
            
    except Exception as exc:
        logger.warning("无法自动播放音频：%s", exc)


async def synthesize_tts_stream(text: str, config: TTSConfig) -> AsyncIterator[bytes]:
    """
    流式生成 TTS 音频数据
    
    Args:
        text: 要合成的文本
        config: TTS 配置
        
    Yields:
        音频数据块
        
    Raises:
        TTSError: 合成失败时抛出
    """
    if edge_tts is None:
        raise TTSError("edge-tts 未安装，无法使用语音合成功能。请运行: pip install edge-tts")
    
    sanitized = text.strip()
    if not sanitized:
        raise TTSError("无法生成空文本的语音。")

    _vlog("开始流式 TTS 合成（文本长度: %d）", len(sanitized))
    
    try:
        communicator = edge_tts.Communicate(
            sanitized,
            voice=config.voice,
            rate=config.rate,
            volume=config.volume,
        )
        
        chunk_count = 0
        async for chunk in communicator.stream():
            if chunk["type"] == "audio" and chunk["data"]:
                chunk_count += 1
                yield chunk["data"]
                await asyncio.sleep(0)  # 让出控制权，避免阻塞
        
        _vlog("流式 TTS 合成完成（共 %d 个音频块）", chunk_count)
        
    except Exception as exc:
        logger.error("流式 TTS 合成失败：%s", exc)
        raise TTSError(f"流式 TTS 合成失败：{exc}") from exc


async def play_audio_stream(audio_stream: AsyncIterator[bytes], config: TTSConfig) -> None:
    """
    实时播放流式音频数据
    
    Args:
        audio_stream: 音频数据流
        config: TTS 配置
        
    Raises:
        TTSError: 播放失败时抛出
    """
    try:
        # 尝试使用 pygame 进行实时播放
        try:
            import pygame
            
            _vlog("使用 pygame 进行流式音频播放")
            pygame.mixer.init(frequency=24000, size=-16, channels=1, buffer=512)
            
            buffer = bytearray()
            chunk_count = 0
            
            async for chunk in audio_stream:
                buffer.extend(chunk)
                chunk_count += 1
                
                # 当缓冲区足够大时播放
                if len(buffer) >= 4096:
                    try:
                        sound = pygame.mixer.Sound(buffer=bytes(buffer))
                        channel = sound.play()
                        
                        # 等待当前块播放完成
                        while channel.get_busy():
                            await asyncio.sleep(0.01)
                        
                        buffer.clear()
                        
                    except Exception as play_exc:
                        _vlog("播放音频块失败: %s", play_exc)
            
            # 播放剩余的缓冲区
            if buffer:
                try:
                    sound = pygame.mixer.Sound(buffer=bytes(buffer))
                    channel = sound.play()
                    while channel.get_busy():
                        await asyncio.sleep(0.01)
                except Exception as play_exc:
                    _vlog("播放最后音频块失败: %s", play_exc)
            
            pygame.mixer.quit()
            _vlog("流式音频播放完成（共 %d 个块）", chunk_count)
            
        except ImportError:
            # pygame 不可用，回退到保存文件并播放
            logger.warning("pygame 不可用，回退到非流式播放模式")
            
            buffer = bytearray()
            async for chunk in audio_stream:
                buffer.extend(chunk)
            
            # 保存到临时文件
            config.output_dir.mkdir(parents=True, exist_ok=True)
            temp_file = config.output_dir / f"temp_stream_{int(time.time() * 1000)}.mp3"
            
            with open(temp_file, "wb") as f:
                f.write(buffer)
            
            _vlog("流式音频已保存到: %s", temp_file)
            
            # 播放保存的文件
            if config.auto_play:
                play_audio(temp_file)
            
    except Exception as exc:
        logger.error("流式音频播放失败：%s", exc)
        raise TTSError(f"流式音频播放失败：{exc}") from exc


def _sys_platform() -> str:
    """获取系统平台"""
    return os.getenv("OSTYPE", "").lower() or os.name


def queue_failed_tts_job(text: str, config: TTSConfig, *, attempt: int = 0) -> None:
    """
    将失败的 TTS 任务加入队列，等待稍后重试
    
    Args:
        text: 要合成的文本
        config: TTS 配置
        attempt: 当前尝试次数
    """
    _FAILED_JOBS.append((text, config, attempt))
    _vlog("TTS 任务已入队待稍后重试（总计 %d 个）", len(_FAILED_JOBS))


async def flush_tts_queue() -> None:
    """
    处理所有待重试的 TTS 任务
    """
    if not _FAILED_JOBS:
        return
    
    pending = list(_FAILED_JOBS)
    _FAILED_JOBS.clear()
    
    _vlog("尝试重放 %d 个待办 TTS 任务", len(pending))
    
    for text, cfg, attempt in pending:
        try:
            await synthesize_tts(text, cfg)
            logger.info("延迟 TTS 任务成功完成")
        except TTSError as exc:
            logger.warning("延迟 TTS 仍失败：%s", exc)
            if attempt + 1 < cfg.max_attempts:
                queue_failed_tts_job(text, cfg, attempt=attempt + 1)


def get_failed_jobs_count() -> int:
    """获取待重试的 TTS 任务数量"""
    return len(_FAILED_JOBS)
