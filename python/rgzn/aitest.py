import asyncio
import io
import numpy as np
import sounddevice as sd
from pydub import AudioSegment
import edge_tts
from g4f.client import Client  # ✅ 新接口

# -------------------------
# 语音模块
# -------------------------
class VoiceEngine:
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
        audio = AudioSegment.from_file(io.BytesIO(audio_data), format="mp3")
        samples = np.array(audio.get_array_of_samples())
        sd.play(samples, audio.frame_rate)
        sd.wait()

# -------------------------
# 对话管理
# -------------------------
class ConversationManager:
    def __init__(self):
        self.history = []
        self.max_history = 20

    def add_message(self, role, content):
        self.history.append({"role": role, "content": content})
        if len(self.history) > self.max_history:
            self.history.pop(0)

    def get_context(self):
        return [{"role": "system", "content": "你是精通时间管理和跨平台操作的智能助理"}] + self.history

# -------------------------
# 任务调度
# -------------------------
class TaskScheduler:
    def __init__(self):
        self.active_tasks = set()

    async def create_task(self, coroutine):
        task = asyncio.create_task(coroutine)
        self.active_tasks.add(task)
        task.add_done_callback(lambda t: self.active_tasks.remove(t))
        return task

# -------------------------
# 主逻辑（仅使用默认 provider）
# -------------------------
async def main_loop():
    voice = VoiceEngine()
    conv_mgr = ConversationManager()
    scheduler = TaskScheduler()

    print("跨平台智能助理已就绪")
    audio_data = await voice.text_to_speech("跨平台智能助理已就绪")
    voice.play_audio(audio_data)

    client = Client()  # 默认 provider

    while True:
        user_input = await asyncio.get_event_loop().run_in_executor(None, input, "您: ")
        if user_input.lower() in ["退出", "exit"]:
            break
        if user_input == "查看记录":
            print(f"\n当前记录：{conv_mgr.history}")
            continue

        conv_mgr.add_message("user", user_input)
        messages = conv_mgr.get_context()

        try:
            res = client.chat.completions.create(
                model="gpt-4o-mini",  # 可换为 gpt-5-mini 等
                messages=messages
            )
            reply = res.choices[0].message.content
        except Exception as e:
            reply = f"请求出错: {e}"

        print("AI:", reply)
        conv_mgr.add_message("assistant", reply)

        # 异步播放语音
        audio_data = await voice.text_to_speech(reply)
        await scheduler.create_task(voice.play_audio(audio_data))

if __name__ == "__main__":
    asyncio.run(main_loop())
