import asyncio
import os
import edge_tts
import subprocess
import time
from openai import OpenAI

# 假设这是您的正确OpenAI API密钥和基础URL
client = OpenAI(
    api_key="YOUR_API_KEY",  # 替换为您的OpenAI API密钥
    base_url="https://api.openai.com/v1"  # 替换为OpenAI官方基础URL
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
    loop = asyncio.get_event_loop_policy().get_event_loop()
    try:
        loop.run_until_complete(amain())
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
    subprocess.call('play-audio test.wav', shell=True)


def read_windows():
    # 播放音频文件
    from pygame import mixer

    mixer.init()
    size = 0.7
    mixer.music.set_volume(size)
    mixer.music.load("./test.mp3")
    mixer.music.play()
    time.sleep(5)


if __name__ == '__main__':
    read_message = "我能做些什么？"
    print(read_message)
    read_windows()
    # while True:
    #     message = input("")
    #     if message.lower() == "退出":
    #         break
    #     read_message = gpt_35_api_stream(message)
    #     read()
