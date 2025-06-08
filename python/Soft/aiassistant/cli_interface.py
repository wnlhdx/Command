import asyncio
from voice_engine import VoiceEngine
from conversation_manager import ConversationManager
from task_scheduler import TaskScheduler
from openai_client import chat_completion
from event_reporter import report_event

async def main_loop():
    voice = VoiceEngine()
    conv_mgr = ConversationManager()
    scheduler = TaskScheduler()

    init_msg = "跨平台智能助理已就绪"
    print(init_msg)
    audio_data = await voice.text_to_speech(init_msg)
    voice.play_audio(audio_data)

    await report_event("session_start", {"init_message": init_msg})

    while True:
        user_input = await asyncio.get_event_loop().run_in_executor(
            None, input, "您: "
        )

        await report_event("user_input", {
            "text_length": len(user_input),
            "text_preview": user_input[:30]
        })

        if user_input.lower() in ["退出", "exit"]:
            await report_event("session_end", {"reason": "用户退出"})
            break

        if user_input == "查看记录":
            await report_event("user_command", {"command": "查看记录"})
            print(f"\n当前记录：{conv_mgr.history}")
            continue

        conv_mgr.add_message("user", user_input)

        response = await chat_completion(conv_mgr.get_context())

        full_reply = []
        async for chunk in response:
            content = chunk.choices[0].delta.content
            if content:
                print(content, end="", flush=True)
                full_reply.append(content)

        assistant_reply = "".join(full_reply)
        conv_mgr.add_message("assistant", assistant_reply)

        await report_event("assistant_reply", {"reply_length": len(assistant_reply)})

        await scheduler.create_task(
            voice.text_to_speech(assistant_reply)
        )

if __name__ == "__main__":
    asyncio.run(main_loop())
