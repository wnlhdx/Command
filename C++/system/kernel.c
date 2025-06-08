// kernel.c
#include <stdint.h>

#define UART0_BASE 0x101f1000

static inline void uart_write(char c) {
    volatile uint32_t* UART0_DR = (volatile uint32_t*)(UART0_BASE + 0x00);
    *UART0_DR = (uint32_t)c;
}

static void uart_puts(const char* s) {
    while (*s) {
        uart_write(*s++);
    }
}

void kernel_main() {
    uart_puts("Hello, OS!\n");

    while (1) {
        // 简单死循环，代表 CPU 空转
    }
}
