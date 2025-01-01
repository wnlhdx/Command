#include <stdint.h>

#define IRQ_ENABLE_ADDR  0x10140010 // IRQ使能寄存器地址
#define IRQ_STATUS_ADDR  0x10140000 // IRQ状态寄存器地址
#define TIMER_LOAD_ADDR  0x101E2000 // 定时器加载值寄存器地址
#define TIMER_CTRL_ADDR  0x101E2008 // 定时器控制寄存器地址
#define TIMER_CLEAR_ADDR 0x101E200C // 定时器清除寄存器地址
#define UART0_BASE 0x101f1000
#define UART0_DR   (*(volatile uint32_t *)(UART0_BASE + 0x00))

// 函数原型声明
void init_interrupts();
void init_timer();
void enable_interrupts();
void isr_irq();
void handle_timer_interrupt();
void uart_putc(char c);
void uart_puts(const char* s);

// 内核主函数
void kernel_main() {
    // 配置中断控制器和定时器
    init_interrupts();
    init_timer();

    // 启用全局中断
    enable_interrupts();

    while (1) {
        // 主循环
    }
}

void uart_putc(char c) {
    while (*(volatile uint32_t*)(UART0_BASE + 0x18) & (1 << 5)); // 等待传输FIFO为空
    UART0_DR = c;  // 发送字符
}

void uart_puts(const char* s) {
    while (*s) {
        uart_putc(*s++);
    }
}


// 初始化中断控制器
void init_interrupts() {
    volatile uint32_t* irq_enable = (volatile uint32_t*)IRQ_ENABLE_ADDR;
    *irq_enable = 1; // 启用 IRQ
}

// 初始化定时器
void init_timer() {
    volatile uint32_t* timer_load = (volatile uint32_t*)TIMER_LOAD_ADDR;
    volatile uint32_t* timer_ctrl = (volatile uint32_t*)TIMER_CTRL_ADDR;

    *timer_load = 0x1000;  // 设置计数值
    *timer_ctrl = 0x7;     // 启用定时器和中断
}

// 中断服务程序
void isr_irq() {
    volatile uint32_t* irq_status = (volatile uint32_t*)IRQ_STATUS_ADDR;
    uint32_t status = *irq_status;

    if (status & (1 << 4)) { // 定时器中断
        handle_timer_interrupt();
    }
}

// 处理定时器中断
void handle_timer_interrupt() {
    volatile uint32_t* timer_clear = (volatile uint32_t*)TIMER_CLEAR_ADDR;
    *timer_clear = 1; // 清除中断标志


    // 输出调试信息
    uart_puts("Timer interrupt handled\n");
    // (可以在此处加入其他中断逻辑)
}
