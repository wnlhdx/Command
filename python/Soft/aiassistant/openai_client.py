import os
from openai import AsyncOpenAI

import dotenv
dotenv.load_dotenv()

client = AsyncOpenAI(
    api_key=os.getenv("OPENAI_API_KEY"),
    base_url=os.getenv("OPENAI_BASE_URL")
)

async def chat_completion(messages, model="gpt-3.5-turbo", stream=True):
    response = await client.chat.completions.create(
        model=model,
        messages=messages,
        stream=stream
    )
    return response
