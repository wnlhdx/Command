// start.S
.section .text
.global _start

_start:
    // 初始化栈（Stack Pointer），假设内存末尾向下增长
    ldr sp, =0x8000

    // 跳转到内核主函数
    bl kernel_main

hang:
    b hang
