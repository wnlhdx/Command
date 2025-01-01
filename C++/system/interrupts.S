.global enable_interrupts
enable_interrupts:
    cpsie i      // 启用 IRQ 中断
    bx lr        // 返回
