from event_reporter import report_event
import asyncio

class TaskScheduler:
    def __init__(self):
        self.active_tasks = set()

    async def create_task(self, coroutine, task_type="generic"):
        task = asyncio.create_task(coroutine)
        self.active_tasks.add(task)

        await report_event("task_created", {
            "task_type": task_type,
            "active_tasks_count": len(self.active_tasks)
        })

        def done_callback(t):
            self.active_tasks.remove(t)
            asyncio.create_task(report_event("task_completed", {
                "task_type": task_type
            }))

        task.add_done_callback(done_callback)
        return task
