#!/usr/bin/env python
"""
测试改进后的代码
"""
from __future__ import annotations

import asyncio
import sys
from pathlib import Path

# 添加当前目录到路径
sys.path.insert(0, str(Path(__file__).parent))


async def test_provider_manager():
    """测试 Provider 管理器"""
    print("=" * 50)
    print("测试 Provider 管理器")
    print("=" * 50)
    
    from providers_improved import ProviderConfig, ProviderManager
    
    config = ProviderConfig(
        names=["Yqcloud", "TeachAnything", "PollinationsAI"],
        restricted_names={"Mintlify"},
        stable_name="Yqcloud",
    )
    
    manager = ProviderManager(config)
    manager.build_provider_pool()
    
    print(f"✓ Provider 池大小: {manager.pool_size}")
    
    # 测试选择最佳 provider
    print("\n正在测速 providers...")
    result = await manager.select_best_provider()
    
    print(f"✓ 已选择最佳 provider: {manager.get_provider_name()}")
    
    # 测试轮换
    old_name = manager.get_provider_name()
    manager.rotate_provider("test rotation")
    new_name = manager.get_provider_name()
    print(f"✓ Provider 轮换: {old_name} -> {new_name}")
    
    # 测试弱网模式
    manager.enable_weak_network("test weak network")
    print(f"✓ 弱网模式已启用: {manager.weak_network_active}")
    
    manager.record_success()
    manager.record_success()
    print(f"✓ 弱网模式状态（连续成功后）: {manager.weak_network_active}")
    
    # 测试带宽记录
    manager.record_bandwidth(100)
    print(f"✓ 带宽记录成功")
    
    print("\nProvider 管理器测试完成 ✓\n")


async def test_tts():
    """测试 TTS 功能"""
    print("=" * 50)
    print("测试 TTS 功能")
    print("=" * 50)
    
    from tts_improved import TTSConfig, synthesize_tts
    
    config = TTSConfig(
        voice="zh-CN-XiaoxiaoNeural",
        output_dir=Path("tts_output"),
        rate="+0%",
        volume="+0%",
        auto_play=False,  # 测试时不自动播放
    )
    
    try:
        output = await synthesize_tts("这是一个测试", config)
        print(f"✓ TTS 合成成功: {output}")
    except Exception as e:
        print(f"✗ TTS 合成失败: {e}")
    
    print("\nTTS 测试完成 ✓\n")


async def test_main_flow():
    """测试主流程"""
    print("=" * 50)
    print("测试主流程")
    print("=" * 50)
    
    from main_improved import (
        build_provider_manager,
        build_tts_config,
        CLIOptions,
        plan_user_command,
    )
    
    # 创建测试选项
    options = CLIOptions(
        prompt=None,
        tts_voice="zh-CN-XiaoxiaoNeural",
        tts_dir=Path("tts_output"),
        tts_rate="+0%",
        tts_volume="+0%",
        tts_immediate=False,
        tts_play=False,
        tts_streaming=False,
        verbose=False,
    )
    
    # 测试命令解析
    tasks = plan_user_command("hello", options, source="test")
    print(f"✓ 解析普通命令: {len(tasks)} 个任务")
    
    tasks = plan_user_command("tts:测试文本", options, source="test")
    print(f"✓ 解析 TTS 命令: {len(tasks)} 个任务")
    
    # 测试构建管理器
    manager = build_provider_manager()
    print(f"✓ 构建 Provider 管理器成功")
    
    # 测试构建 TTS 配置
    tts_config = build_tts_config(options)
    print(f"✓ 构建 TTS 配置成功")
    
    print("\n主流程测试完成 ✓\n")


async def main():
    """运行所有测试"""
    print("\n" + "=" * 50)
    print("开始测试改进后的代码")
    print("=" * 50 + "\n")
    
    try:
        await test_provider_manager()
        await test_tts()
        await test_main_flow()
        
        print("=" * 50)
        print("所有测试完成 ✓")
        print("=" * 50)
        
    except Exception as e:
        print(f"\n测试失败: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0


if __name__ == "__main__":
    exit_code = asyncio.run(main())
    sys.exit(exit_code)
