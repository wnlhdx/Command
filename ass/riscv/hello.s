.section .data
msg:
    .asciz "Hello, RISC-V!\n"

.section .text
.globl _start
_start:
    # syscall: write (stdout)
    li a7, 64         # syscall number for write
    li a0, 1          # file descriptor 1 (stdout)
    la a1, msg        # address of the message
    li a2, 15         # message length
    ecall

    # syscall: exit
    li a7, 93         # syscall number for exit
    li a0, 0          # exit code 0
    ecall
