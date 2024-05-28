; 假设使用的汇编器是arm-none-eabi-as
; 假设链接器是arm-none-eabi-ld

.section .text, "ax", @progbits
.global _start
_start:
    mov r0, #0x00000000      ; 设置栈指针
    mov sp, r0

    mov r0, #0x00000000      ; 设置堆指针
    mov r1, r0

    ; 初始化寄存器
    mov r2, #0x10000000      ; 内存基地址
    mov r3, #0x00000000      ; 内存大小
    mov r4, #0x00000000      ; 内存大小
    mov r5, #0x00000000      ; 内存大小

    ; 初始化GDT
    mov r0, #0x00000000      ; GDT基地址
    mov r1, #0x00000000      ; GDT界限
    mov r2, #0x00000000      ; GDT描述符属性
    mov r3, #0x00000000      ; GDT描述符属性
    mov r4, #0x00000000      ; GDT描述符属性
    mov r5, #0x00000000      ; GDT描述符属性

    ; 初始化IDT
    mov r0, #0x00000000      ; IDT基地址
    mov r1, #0x00000000      ; IDT界限
    mov r2, #0x00000000      ; IDT描述符属性
    mov r3, #0x00000000      ; IDT描述符属性
    mov r4, #0x00000000      ; IDT描述符属性
    mov r5, #0x00000000      ; IDT描述符属性

    ; 设置CR0和CR3寄存器
    mov r0, #0x00000000      ; CR0寄存器值
    mov r1, #0x00000000      ; CR3寄存器值

    ; 切换到保护模式
    lgdt [gdt]
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    lidt [idt]
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    mov cr3, r2

    ; 跳转到32位代码
    jmp 0x08 : 32bit_code

; 32位代码段
32bit_code:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 这里添加你的32位代码

    ; 返回BIOS中断
    mov ax, 0x16
    int 0x16

; GDT描述符
gdt:
    .word 0x0000, 0x0000
    .word 0x0000, 0x0000
    .word 0x0000, 0x0000
