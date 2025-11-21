import asyncio, random

async def simulate_call(name: str) -> str:
    await asyncio.sleep(random.uniform(0.1, 0.5))  # 模拟网络延迟
    if random.random() < 0.2:
        raise Exception(f"{name} 失败")
    return f"{name} 回复"

async def main():
    # 创建多个协程任务，不 await 立即执行
    tasks = [
        asyncio.create_task(simulate_call("provider1")),
        asyncio.create_task(simulate_call("provider2")),
        asyncio.create_task(simulate_call("provider3")),
    ]

    # 等待所有任务完成
    results = await asyncio.gather(*tasks, return_exceptions=True)
    
    for r in results:
        if isinstance(r, Exception):
            print("出错:", r)
        else:
            print(r)

if __name__ == "__main__":
    asyncio.run(main())