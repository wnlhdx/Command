import time
import asyncio
import aiohttp

async def report_event(event_name: str, data: dict):
    event = {
        "event": event_name,
        "timestamp": time.time(),
        "data": data
    }
    # 这里先简单打印，正式环境可改成真实上报
    print(f"[Event] {event_name}: {data}")

    # 模拟异步发送
    try:
        async with aiohttp.ClientSession() as session:
            await session.post("https://your-analytics-endpoint.com/events", json=event)
    except Exception as e:
        print(f"事件上报失败: {e}")
