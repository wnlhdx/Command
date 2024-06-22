import asyncio
import contextlib
import os
import sys

import edge_tts
import subprocess
import time
import importlib
from openai import OpenAI

# 假设这是您的正确OpenAI API密钥和基础URL
client = OpenAI(
    api_key="sk-VjSCWObgSc3EbfaJQRACXMvb33Q7th40lxF9d7Sk9aJoydQ8",  # 替换为您的OpenAI API密钥
    base_url="https://api.chatanywhere.tech/v1"  # 替换为OpenAI官方基础URL
)


def gpt_35_api_stream(input_message):
    completion = client.chat.completions.create(
        model="gpt-3.5-turbo",
        messages=[
            {"role": "system",
             "content": "You are a personal assistant, skilled in all things about a person who wants to know.And I "
                        "will use you to help me compelet all my work"},
            {"role": "user", "content": input_message}
        ]
    )
    messages = completion.choices[0].message.content
    print(messages)
    return messages


def save():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    try:
        asyncio.run(amain())
    finally:
        loop.close()


async def amain() -> None:
    """Xiaoxiao或者Xiaoyi"""
    communicate = edge_tts.Communicate(read_message, 'zh-CN-XiaoxiaoNeural')
    await communicate.save('test.mp3')


def read():
    save()
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        read_termux()
    elif os.name == 'nt':
        # Windows 系统
        read_windows()


def read_termux():
    # 播放音频文件
    subprocess.call('mpv test.mp3', shell=True)


def read_windows():
    # 播放音频文件
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


if __name__ == '__main__':
    read_message = "初始化中，请问我能做些什么？"
    print(read_message)
    read()
    while True:
        message = input("")
        if message.lower() == "退出":
            break
        read_message = gpt_35_api_stream(message)
        read()
