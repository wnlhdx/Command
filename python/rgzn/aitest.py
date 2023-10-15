import openai
import edge_tts
from edge_tts import VoicesManager
import asyncio
import requests

openai.api_base = "https://api.chatanywhere.com.cn/v1"
openai.api_key = "sk-VjSCWObgSc3EbfaJQRACXMvb33Q7th40lxF9d7Sk9aJoydQ8"


def gpt_35_api_stream(message):
    try:
        messages = [{'role': 'user', 'content': message}, ]
        response = openai.ChatCompletion.create(
            model='gpt-3.5-turbo',
            messages=messages,
            stream=True,
        )
        completion = {'role': '', 'content': ''}
        for event in response:
            if event['choices'][0]['finish_reason'] == 'stop':
                # print(f'收到的完成数据: {completion}')
                # print(f'openai返回结果:{completion.get("content")}')
                break
            for delta_k, delta_v in event['choices'][0]['delta'].items():
                # print(f'流响应数据: {delta_k} = {delta_v}')
                completion[delta_k] += delta_v
        messages.append(completion)  # 直接在传入参数 messages 中追加消息
        # return (True, '')
        return completion.get("content")
    except Exception as err:
        return '连接openai失败'
        # return (False, f'OpenAI API 异常: {err}')


async def amain(TEXT) -> None:
    """Main function"""
    voices = await VoicesManager.create()
    # voice = voices.find(Gender="Female", Locale="zh-CN")
    # communicate = edge_tts.Communicate(TEXT, "zh-CN-XiaoxiaoNeural")
    communicate = edge_tts.Communicate(TEXT, "zh-CN-XiaoyiNeural")
    # voice = voices.find(Gender="Female", Locale="es-AR")
    # communicate = edge_tts.Communicate(TEXT, random.choice(voice)["Name"])
    await communicate.save("output.mp3")


def tts(text):
    loop = asyncio.get_event_loop_policy().get_event_loop()
    try:
        loop.run_until_complete(amain(text))
    finally:
        loop.close()


if __name__ == '__main__':
    speaker = '世界上最优秀的化学教材'
    responser = gpt_35_api_stream(speaker)
    print(responser)
    url = f'http://genshinvoice.top/api'
    param = {'speaker': '克拉拉', 'text': responser}

    # tts(responser)
    open('test.wav', 'wb').write(requests.get(url, param).content)
