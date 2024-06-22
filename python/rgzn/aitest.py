from openai import OpenAI
import os
import requests

client = OpenAI(
    api_key="sk-VjSCWObgSc3EbfaJQRACXMvb33Q7th40lxF9d7Sk9aJoydQ8",
    base_url="https://api.chatanywhere.tech/v1"
)


def gpt_35_api_stream(input_message):
    completion = client.chat.completions.create(
        model="gpt-3.5-turbo",
        messages=[
            {"role": "system",
             "content": "You are a personal assistant, skilled in all things about a person who wants to know.And I will use you to help me compelet all my work"},
            {"role": "user", "content": input_message}
        ]
    )
    return completion.choices[0].message.content


def read():
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        read_termux()
    elif os.name == 'nt':
        # Windows 系统
        read_windows()


def read_termux():
    return 0


def read_windows():
    return 0


if __name__ == '__main__':
    print("我能做些什么？")
    while 1 == 1:
        message = input("")
        print(gpt_35_api_stream(message))

