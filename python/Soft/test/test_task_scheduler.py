# test_task_scheduler.py

import pytest
import asyncio
from Soft.aiassistant.task_scheduler import  TaskScheduler  # 替换

@pytest.mark.asyncio
async def test_create_and_remove_task():
    scheduler = TaskScheduler()
    async def dummy():
        await asyncio.sleep(0.01)
        return "done"

    task = await scheduler.create_task(dummy())
    assert task in scheduler.active_tasks
    await task
    # 任务完成后应被移除
    assert task not in scheduler.active_tasks
