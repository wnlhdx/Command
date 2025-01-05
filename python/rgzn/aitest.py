import asyncio
import os
import subprocess
import time
import importlib
from openai import OpenAI
import edge_tts

# 假设这是您的正确OpenAI API密钥和基础URL
client = OpenAI(
    api_key="sk-VjSCWObgSc3EbfaJQRACXMvb33Q7th40lxF9d7Sk9aJoydQ8",  # 替换为您的OpenAI API密钥
    base_url="https://api.chatanywhere.tech/v1"  # 替换为OpenAI官方基础URL
)

# 用于保存对话记录的列表
conversation_log = []


def gpt_35_api_stream(input_message):
    """调用 GPT-3.5 Turbo API，返回生成的消息内容"""
    completion = client.chat.completions.create(
        model="gpt-3.5-turbo",
        messages=[
            {"role": "system",
             "content": "You are a personal assistant, skilled in all things about a person who wants to know. "
                        "And I will use you to help me complete all my work."},
            {"role": "user", "content": input_message}
        ]
    )
    messages = completion.choices[0].message.content
    print(messages)
    return messages


async def amain(read_message: str) -> None:
    """通过 edge_tts 将文本转换为语音并保存为 MP3 文件"""
    communicate = edge_tts.Communicate(read_message, 'zh-CN-XiaoxiaoNeural')
    await communicate.save('test.mp3')


def save(read_message: str):
    """封装 amain 方法，保存语音文件"""
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    try:
        loop.run_until_complete(amain(read_message))
    finally:
        loop.close()


def read(read_message: str):
    """保存并播放语音"""
    save(read_message)
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        read_termux()
    elif os.name == 'nt':
        # Windows 系统
        read_windows()


def read_termux():
    """通过 mpv 播放音频文件（适用于 Unix-like 系统）"""
    subprocess.call('mpv test.mp3', shell=True)


def read_windows():
    """通过 pygame 播放音频文件（适用于 Windows 系统）"""
    try:
        # 动态导入 pygame，避免未安装时报错
        module = importlib.import_module("pygame")
        module.mixer.init()
        size = 0.7
        module.mixer.music.set_volume(size)
        module.mixer.music.load("test.mp3")
        module.mixer.music.play()
        # 等待播放完成
        while module.mixer.music.get_busy():
            time.sleep(0.1)
        # 确保文件不再被播放器占用
        module.mixer.music.unload()
    except ModuleNotFoundError:
        print("请安装 pygame 模块以支持语音播放：pip install pygame")
        sys.exit(1)


def save_conversation(user_message, ai_reply):
    """将用户输入和 AI 回复保存到列表中"""
    conversation_log.append({"用户": user_message, "AI": ai_reply})


def view_conversation():
    """查看对话记录"""
    if conversation_log:
        print("\n当前对话记录：")
        for index, entry in enumerate(conversation_log, 1):
            print(f"记录 {index}:")
            print(f"用户: {entry['用户']}")
            print(f"AI: {entry['AI']}")
            print("=" * 50)
    else:
        print("暂无对话记录。")


def delete_conversation():
    """清空对话记录列表"""
    conversation_log.clear()
    print("对话记录已清空。")


if __name__ == '__main__':
    # 初始化提示语
    initial_message = "初始化中，请问我能做些什么？"
    print(initial_message)
    read(initial_message)
    while True:
        # 获取用户输入
        user_message = input("用户: ")
        if user_message.lower() == "退出":
            break
        elif user_message.lower() == "查看记录":
            view_conversation()
            continue
        elif user_message.lower() == "清空记录":
            delete_conversation()
            continue

        # 调用 GPT-3.5 API 获取回复
        ai_reply = gpt_35_api_stream(user_message)

        # 保存对话记录到列表
        save_conversation(user_message, ai_reply)

        # 播放 GPT 的回复语音
        read(ai_reply)
