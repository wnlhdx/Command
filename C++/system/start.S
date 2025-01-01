.section .text
.global _start

_start:
    // 设置中断向量表
    ldr pc, =reset_handler     // Reset
    ldr pc, =undef_handler     // Undefined instruction
    ldr pc, =svc_handler       // Software interrupt (SVC)
    ldr pc, =prefetch_handler  // Prefetch abort
    ldr pc, =data_handler      // Data abort
    ldr pc, =unused_handler    // Unused
    ldr pc, =irq_handler       // IRQ
    ldr pc, =fiq_handler       // FIQ

reset_handler:
    bl kernel_main             // 跳转到内核主程序
    b .

undef_handler:
    b .
svc_handler:
    b .
prefetch_handler:
    b .
data_handler:
    b .
unused_handler:
    b .
irq_handler:
    // 保存上下文
    sub lr, lr, #4             // 调整 lr 为中断发生地址
    stmfd sp!, {r0-r12, lr}    // 保存寄存器
    bl isr_irq                 // 调用 C 的中断处理函数
    ldmfd sp!, {r0-r12, pc}^   // 恢复寄存器
fiq_handler:
    b .
