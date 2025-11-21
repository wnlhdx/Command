import asyncio
import random
import time

from provider import ProviderManager


async def tts(name: str) -> str:
    print(f"[{time.time():.2f}] {name} start")
    await asyncio.sleep(random.uniform(1, 20))
    print(f"[{time.time():.2f}] {name} end")
    return f"{name}.mp3 generated"


async def queue(q: asyncio.Queue):
    while True:
        task = await q.get()
        if task is None:
            q.task_done()
            break
        if task["type"] == "chat":
            print(f"start chat job:{task}")
            pm = ProviderManager(task["providers"])
            try:
                res = await ProviderManager.wait_success(pm)
                print(f"res:{res}")
                await  q.put({"type": "tts", "text": res})
            except Exception as e:
                print(f"chat{task} failed:{e}")
        elif task["type"] == "tts":
            print(f"start tts job:{task}")
            try:
                path = await tts(task["text"])
                print(f"tts{task} completed")
            except Exception as e:
                print(f"tts{task} failed:{e}")
        q.task_done()


async def main():
    q = asyncio.Queue()
    await q.put({"type": "chat", "providers": ["p1", "p2", "p3"]})
    w = asyncio.create_task(queue(q))
    await q.join()
    await q.put(None)
    await w


if __name__ == "__main__":
    asyncio.run(main())
