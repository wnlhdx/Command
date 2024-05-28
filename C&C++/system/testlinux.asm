; ����ʹ�õĻ������arm-none-eabi-as
; ������������arm-none-eabi-ld

.section .text, "ax", @progbits
.global _start
_start:
    mov r0, #0x00000000      ; ����ջָ��
    mov sp, r0

    mov r0, #0x00000000      ; ���ö�ָ��
    mov r1, r0

    ; ��ʼ���Ĵ���
    mov r2, #0x10000000      ; �ڴ����ַ
    mov r3, #0x00000000      ; �ڴ��С
    mov r4, #0x00000000      ; �ڴ��С
    mov r5, #0x00000000      ; �ڴ��С

    ; ��ʼ��GDT
    mov r0, #0x00000000      ; GDT����ַ
    mov r1, #0x00000000      ; GDT����
    mov r2, #0x00000000      ; GDT����������
    mov r3, #0x00000000      ; GDT����������
    mov r4, #0x00000000      ; GDT����������
    mov r5, #0x00000000      ; GDT����������

    ; ��ʼ��IDT
    mov r0, #0x00000000      ; IDT����ַ
    mov r1, #0x00000000      ; IDT����
    mov r2, #0x00000000      ; IDT����������
    mov r3, #0x00000000      ; IDT����������
    mov r4, #0x00000000      ; IDT����������
    mov r5, #0x00000000      ; IDT����������

    ; ����CR0��CR3�Ĵ���
    mov r0, #0x00000000      ; CR0�Ĵ���ֵ
    mov r1, #0x00000000      ; CR3�Ĵ���ֵ

    ; �л�������ģʽ
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

    ; ��ת��32λ����
    jmp 0x08 : 32bit_code

; 32λ�����
32bit_code:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; ����������32λ����

    ; ����BIOS�ж�
    mov ax, 0x16
    int 0x16

; GDT������
gdt:
    .word 0x0000, 0x0000
    .word 0x0000, 0x0000
    .word 0x0000, 0x0000
