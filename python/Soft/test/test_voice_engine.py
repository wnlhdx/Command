# test_voice_engine.py

import pytest
import asyncio
from unittest import mock
from Soft.aiassistant.voice_engine import VoiceEngine  # 替换为实际模块名

@pytest.mark.asyncio
async def test_text_to_speech_streaming():
    fake_audio_chunks = [
        {"type": "audio", "data": b"chunk1"},
        {"type": "audio", "data": b"chunk2"},
        {"type": "other", "data": b"ignore"},
        {"type": "audio", "data": b"chunk3"},
    ]

    async def fake_stream():
        for chunk in fake_audio_chunks:
            yield chunk

    with mock.patch("edge_tts.Communicate.stream", new=lambda self: fake_stream()), \
         mock.patch("edge_tts.Communicate.__init__", return_value=None):
        audio_bytes = await VoiceEngine.text_to_speech("测试文本")
        assert audio_bytes == b"chunk1chunk2chunk3"

@mock.patch("sounddevice.play")
@mock.patch("sounddevice.wait")
@mock.patch("pydub.AudioSegment.from_file")
def test_play_audio(mock_from_file, mock_wait, mock_play):
    # 准备模拟AudioSegment实例
    mock_audio = mock.Mock()
    mock_audio.get_array_of_samples.return_value = [1, 2, 3, 4]
    mock_audio.frame_rate = 44100
    mock_from_file.return_value = mock_audio

    VoiceEngine.play_audio(b"fake audio data")

    mock_from_file.assert_called_once()
    mock_audio.get_array_of_samples.assert_called_once()
    mock_play.assert_called_once()
    mock_wait.assert_called_once()
