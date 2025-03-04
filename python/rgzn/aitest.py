import asyncio
import os
import sys
import io
from typing import Optional
from openai import AsyncOpenAI  # 使用异步客户端
import sounddevice as sd  # 跨平台音频播放
import numpy as np
from pydub import AudioSegment
import edge_tts

# 安全配置管理（使用python-dotenv）
from dotenv import load_dotenv
load_dotenv()

# 初始化异步客户端
client = AsyncOpenAI(
    api_key=os.getenv("sk-VjSCWObgSc3EbfaJQRACXMvb33Q7th40lxF9d7Sk9aJoydQ8"),
    base_url=os.getenv("https://api.chatanywhere.tech/v1")
)

class VoiceEngine:
    """跨平台语音引擎（内存流处理）"""
    @staticmethod
    async def text_to_speech(text: str):
        communicate = edge_tts.Communicate(text, 'zh-CN-XiaoxiaoNeural')
        audio_stream = bytearray()
        async for chunk in communicate.stream():
            if chunk["type"] == "audio":
                audio_stream.extend(chunk["data"])
        return audio_stream

    @staticmethod
    def play_audio(audio_data: bytes):
        """使用sounddevice播放音频"""
        audio = AudioSegment.from_file(io.BytesIO(audio_data), format="mp3")
        samples = np.array(audio.get_array_of_samples())
        sd.play(samples, audio.frame_rate)
        sd.wait()

class ConversationManager:
    """对话管理系统"""
    def __init__(self):
        self.history = []
        self.max_history = 20  # 保持最近20条记录

    def add_message(self, role: str, content: str):
        self.history.append({"role": role, "content": content})
        if len(self.history) > self.max_history:
            self.history.pop(0)

    def get_context(self):
        return [{"role": "system", "content": "你是精通时间管理和跨平台操作的智能助理"}] + self.history

class TaskScheduler:
    """后台任务调度器"""
    def __init__(self):
        self.active_tasks = set()

    async def create_task(self, coroutine):
        """创建并跟踪异步任务"""
        task = asyncio.create_task(coroutine)
        self.active_tasks.add(task)
        task.add_done_callback(lambda t: self.active_tasks.remove(t))
        return task

async def main_loop():
    # 初始化核心组件
    voice = VoiceEngine()
    conv_mgr = ConversationManager()
    scheduler = TaskScheduler()

    # 启动初始化问候
    init_msg = "跨平台智能助理已就绪"
    print(init_msg)
    audio_data = await voice.text_to_speech(init_msg)
    voice.play_audio(audio_data)

    while True:
        # 异步获取用户输入
        user_input = await asyncio.get_event_loop().run_in_executor(
            None, input, "您: "
        )

        if user_input.lower() in ["退出", "exit"]:
            break

        # 处理特殊命令
        if user_input == "查看记录":
            print(f"\n当前记录：{conv_mgr.history}")
            continue

        # 添加用户消息到上下文
        conv_mgr.add_message("user", user_input)

        # 异步调用GPT
        response = await client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=conv_mgr.get_context(),
            stream=True
        )

        # 流式处理回复
        full_reply = []
        async for chunk in response:
            content = chunk.choices[0].delta.content
            if content:
                print(content, end="", flush=True)
                full_reply.append(content)

        # 添加AI回复到上下文
        conv_mgr.add_message("assistant", "".join(full_reply))

        # 异步播放语音（不阻塞主循环）
        await scheduler.create_task(
            voice.text_to_speech("".join(full_reply))
        )

if __name__ == "__main__":
    asyncio.run(main_loop())