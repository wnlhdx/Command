# test_conversation_manager.py

import pytest
from Soft.aiassistant.conversation_manager import ConversationManager  # 替换

def test_add_message_and_max_history():
    cm = ConversationManager()
    for i in range(25):
        cm.add_message("user", f"msg {i}")
    # 最大只保留20条
    assert len(cm.history) == 20
    # 最早的5条被移除，剩余为msg 5 ~ msg 24
    assert cm.history[0]["content"] == "msg 5"

async def test_get_context():
    cm = ConversationManager()
    await  cm.add_message("user", "hello")
    context = cm.get_context()
    assert context[0]["role"] == "system"
    assert context[0]["content"].startswith("你是")
    assert context[1]["role"] == "user"
    assert context[1]["content"] == "hello"
