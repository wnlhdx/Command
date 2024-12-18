.section .data
msg: .asciz "Hello, AArch64!\n"

.section .text
.global _start
_start:
    ; syscall: write (stdout)
    mov x0, 1          ; file descriptor 1 (stdout)
    ldr x1, =msg       ; address of the message
    mov x2, 15         ; message length
    mov x8, 64         ; syscall number for write
    svc 0              ; make syscall

    ; syscall: exit
    mov x0, 0          ; exit code 0
    mov x8, 93         ; syscall number for exit
    svc 0              ; make syscall