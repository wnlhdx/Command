from __future__ import annotations

import abc
import asyncio
import json
import logging
import math
import time
from dataclasses import dataclass, field
from typing import Any, AsyncIterator, Dict, List, Optional, Tuple

import g4f

logger = logging.getLogger(__name__)

Messages = List[Dict[str, str]]

g4f.debug.logging = False
g4f.check_version = False

_VERBOSE = False


def set_llm_verbose(flag: bool) -> None:
    """Allow the CLI to toggle verbose tracing for educational purposes."""
    global _VERBOSE
    _VERBOSE = flag
    logger.setLevel(logging.DEBUG if flag else logging.INFO)


def _vlog(message: str, *args: Any) -> None:
    if _VERBOSE:
        logger.debug(message, *args)


class LLMGenerationError(Exception):
    """Base exception raised when the LLM backend fails."""


class EmptyResponseError(LLMGenerationError):
    """Raised when the backend returns an empty response."""


class ProviderUnavailableError(LLMGenerationError):
    """Raised when the configured provider is unavailable."""


class LLMProvider(abc.ABC):
    """Abstract base for all language-model backends."""

    @abc.abstractmethod
    async def generate(self, messages: Messages) -> str:
        """Generate a response for the given conversation."""

    @abc.abstractmethod
    async def stream(self, messages: Messages) -> AsyncIterator[str]:
        """Stream responses for the given conversation."""

    @abc.abstractmethod
    async def health_check(self) -> bool:
        """Return True when the backend is healthy."""


class LocalLLMProvider(LLMProvider):
    """Placeholder for future llama.cpp integration."""

    async def generate(self, messages: Messages) -> str:
        raise NotImplementedError("Local LLM provider not implemented yet.")

    async def stream(self, messages: Messages) -> AsyncIterator[str]:
        raise NotImplementedError("Local LLM provider not implemented yet.")

    async def health_check(self) -> bool:
        return False


class EdgeLLMProvider(LLMProvider):
    """Placeholder for future TERMUX-based edge inference."""

    async def generate(self, messages: Messages) -> str:
        raise NotImplementedError("Edge LLM provider not implemented yet.")

    async def stream(self, messages: Messages) -> AsyncIterator[str]:
        raise NotImplementedError("Edge LLM provider not implemented yet.")

    async def health_check(self) -> bool:
        return False


class G4FRemoteProvider(LLMProvider):
    """LLM provider backed by g4f remote services."""

    def __init__(
        self,
        provider_cls: Any,
        *,
        model: Any = g4f.models.default,
        request_timeout: int = 20,
        max_retries: int = 3,
        verbose: bool | None = None,
    ) -> None:
        self._provider_cls = provider_cls
        self._model = model
        self._request_timeout = request_timeout
        self._max_retries = max_retries
        self._verbose = _VERBOSE if verbose is None else verbose

    async def generate(self, messages: Messages) -> str:
        delay = 1
        last_error: Optional[Exception] = None
        for attempt in range(self._max_retries):
            try:
                _vlog(
                    "g4f generate attempt %s with provider %s",
                    attempt + 1,
                    self.provider_name,
                )
                response = await asyncio.wait_for(
                    g4f.ChatCompletion.create_async(
                        model=self._model,
                        provider=self._provider_cls,
                        messages=messages,
                        timeout=self._request_timeout,
                    ),
                    timeout=self._request_timeout + 5,
                )
                if self._verbose:
                    _vlog("Raw provider response: %s", response)
                text = extract_text(response, verbose=self._verbose)
                if not text.strip():
                    raise EmptyResponseError(
                        f"{self.provider_name} returned an empty response."
                    )
                return text
            except Exception as exc:  # noqa: BLE001
                last_error = exc
                logger.warning(
                    "g4f provider %s failed attempt %s/%s: %s",
                    self.provider_name,
                    attempt + 1,
                    self._max_retries,
                    exc,
                )
                if attempt < self._max_retries - 1:
                    await asyncio.sleep(delay)
                    delay *= 2
                    continue
                raise ProviderUnavailableError(
                    f"Provider {self.provider_name} is unavailable."
                ) from last_error
        raise ProviderUnavailableError(
            f"Provider {self.provider_name} is unavailable."
        ) from last_error

    async def stream(self, messages: Messages) -> AsyncIterator[str]:
        yield await self.generate(messages)

    async def health_check(self) -> bool:
        try:
            await self.generate([{"role": "user", "content": "ping"}])
            return True
        except LLMGenerationError as exc:
            logger.debug("health_check failed for %s: %s", self.provider_name, exc)
            return False

    @property
    def provider_name(self) -> str:
        return getattr(self._provider_cls, "__name__", str(self._provider_cls))


def _extract_from_json(data: Dict[str, Any]) -> str:
    choices = data.get("choices")
    if not isinstance(choices, list) or not choices:
        _vlog("extract_text: no choices in response json")
        return ""
    choice = choices[0]
    if not isinstance(choice, dict):
        _vlog("extract_text: first choice is not a dict (%s)", type(choice))
        return ""
    message = choice.get("message")
    if isinstance(message, dict):
        content = message.get("content")
        if isinstance(content, str) and content.strip():
            _vlog("extract_text: extracted content from message")
            return content.strip()
        _vlog("extract_text: message content missing or empty")
    delta = choice.get("delta")
    if isinstance(delta, dict):
        content = delta.get("content")
        if isinstance(content, str) and content.strip():
            _vlog("extract_text: extracted content from delta")
            return content.strip()
        _vlog("extract_text: delta content missing or empty")
    _vlog("extract_text: no usable content in JSON choices")
    return ""


def _extract_text_after_json(raw: str) -> str:
    closing_index = raw.rfind("}")
    if closing_index == -1 or closing_index == len(raw) - 1:
        _vlog("extract_text: no trailing text after JSON payload")
        return ""
    tail = raw[closing_index + 1 :].strip()
    if tail:
        _vlog("extract_text: extracted trailing text after JSON payload")
    else:
        _vlog("extract_text: trailing text after JSON payload empty")
    return tail


def extract_text(resp: Any, *, verbose: bool | None = None) -> str:
    local_verbose = _VERBOSE if verbose is None else verbose
    if local_verbose:
        logger.debug("extract_text: response type %s", type(resp))
    if resp is None:
        logger.warning("extract_text: response is None")
        return ""
    if isinstance(resp, str):
        if local_verbose:
            logger.debug("extract_text: handling string response length %s", len(resp))
        cleaned = resp.strip()
        if not cleaned:
            logger.warning("extract_text: string response empty after strip")
            return ""
        try:
            data = json.loads(cleaned)
            if local_verbose:
                logger.debug("extract_text: parsed JSON string successfully")
            extracted = _extract_from_json(data)
            return extracted or cleaned
        except json.JSONDecodeError as exc:
            if local_verbose:
                logger.debug("extract_text: json decode failed: %s", exc)
            trailing = _extract_text_after_json(cleaned)
            return trailing or cleaned
    if isinstance(resp, dict):
        if local_verbose:
            logger.debug("extract_text: handling dict response with keys %s", list(resp.keys()))
        extracted = _extract_from_json(resp)
        if extracted:
            return extracted
        logger.warning("extract_text: dict response missing usable content")
        return ""
    logger.warning("extract_text: unsupported response type %s", type(resp))
    return ""


def estimate_tokens(messages: Messages, completion: str) -> int:
    prompt_chars = sum(len(msg.get("content", "")) for msg in messages)
    completion_chars = len(completion)
    total_chars = prompt_chars + completion_chars
    estimated = max(1, math.ceil(total_chars / 4))
    logger.debug(
        "estimate_tokens: prompt_chars=%s completion_chars=%s -> %s tokens",
        prompt_chars,
        completion_chars,
        estimated,
    )
    return estimated


@dataclass
class BandwidthPolicy:
    limit_per_minute: int
    window_seconds: int = 60
    _usage: Dict[str, List[Tuple[float, int]]] = field(default_factory=dict)

    def record(self, provider_name: str, tokens: int) -> None:
        now = time.monotonic()
        provider_usage = self._usage.setdefault(provider_name, [])
        provider_usage.append((now, tokens))
        self._usage[provider_name] = [
            (ts, value)
            for ts, value in provider_usage
            if now - ts <= self.window_seconds
        ]
        logger.debug(
            "BandwidthPolicy.record: provider=%s tokens=%s total_window=%s",
            provider_name,
            tokens,
            self.total(provider_name),
        )

    def total(self, provider_name: str) -> int:
        provider_usage = self._usage.get(provider_name, [])
        return sum(value for _, value in provider_usage)

    def over_limit(self, provider_name: str) -> bool:
        total_tokens = self.total(provider_name)
        over = total_tokens >= self.limit_per_minute
        if over:
            logger.info(
                "BandwidthPolicy: provider %s exceeded limit (%s/%s)",
                provider_name,
                total_tokens,
                self.limit_per_minute,
            )
        return over

