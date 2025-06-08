from event_reporter import report_event
import asyncio

class ConversationManager:
    def __init__(self):
        self.history = []
        self.max_history = 20

    async def add_message(self, role: str, content: str):
        self.history.append({"role": role, "content": content})
        if len(self.history) > self.max_history:
            self.history.pop(0)

        await report_event("context_update", {
            "role": role,
            "content_length": len(content),
            "history_length": len(self.history)
        })

    def get_context(self):
        return [{"role": "system", "content": "你是精通时间管理和跨平台操作的智能助理"}] + self.history
