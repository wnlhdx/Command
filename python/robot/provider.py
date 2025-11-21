import asyncio
import random
import time


async def call_provider(name: str) -> str:
    print(f"[{time.time():.2f}] {name} start")
    await asyncio.sleep(random.uniform(1, 10))
    print(f"[{time.time():.2f}] {name} end")
    if random.random() < 0.5:
        raise Exception(f"{name} Connect False")
    return f"{name} Connected"


class ProviderManager:
    def __init__(self, providers, timeout=8):
        self.providers = providers
        self.timeout = timeout

    async def wait_success(self):
        tasks = []
        loop = asyncio.get_running_loop()
        for p in self.providers:
            coro = call_provider(p)
            t = loop.create_task(asyncio.wait_for(coro, timeout=self.timeout))
            tasks.append(t)

        try:
            for completed in asyncio.as_completed(tasks):
                try:
                    res = await  completed
                    for t in tasks:
                        if not t.done():
                            t.cancel()
                    return res
                except TimeoutError:
                    # this provider errored/timeout -> try next completed
                    continue
            raise Exception("no accessible provider")
        finally:
            for t in tasks:
                if not t.done():
                    t.cancel()
