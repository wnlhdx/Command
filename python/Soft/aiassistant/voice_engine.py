import io
import numpy as np
import sounddevice as sd
from pydub import AudioSegment
import edge_tts
import time
import asyncio
from event_reporter import report_event

class VoiceEngine:
    @staticmethod
    async def text_to_speech(text: str):
        start_time = time.time()
        communicate = edge_tts.Communicate(text, 'zh-CN-XiaoxiaoNeural')
        audio_stream = bytearray()
        async for chunk in communicate.stream():
            if chunk["type"] == "audio":
                audio_stream.extend(chunk["data"])
        duration = time.time() - start_time

        await report_event("tts_request", {
            "text_length": len(text),
            "duration_sec": duration
        })

        return audio_stream

    @staticmethod
    def play_audio(audio_data: bytes):
        start_time = time.time()
        audio = AudioSegment.from_file(io.BytesIO(audio_data), format="mp3")
        samples = np.array(audio.get_array_of_samples())
        sd.play(samples, audio.frame_rate)
        sd.wait()
        duration = time.time() - start_time
        # 这里 play_audio 是同步函数，异步事件上报用 asyncio.create_task 包装
        asyncio.create_task(report_event("audio_playback", {
            "play_duration_sec": duration
        }))
