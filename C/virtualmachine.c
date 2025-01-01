#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 1024  // 内存大小
#define REG_COUNT 16   // 寄存器数量

// 虚拟机结构体，包含寄存器和内存
typedef struct {
    uint32_t regs[REG_COUNT];  // 16个寄存器
    uint8_t memory[MEM_SIZE];  // 内存
} VirtualMachine;

// 初始化虚拟机
void init_vm(VirtualMachine* vm) {
    for (int i = 0; i < REG_COUNT; i++) {
        vm->regs[i] = 0;  // 所有寄存器初始化为0
    }
    for (int i = 0; i < MEM_SIZE; i++) {
        vm->memory[i] = 0;  // 所有内存初始化为0
    }
}

// MOV指令：将值加载到寄存器中
void exec_MOV(VirtualMachine* vm, int reg, uint32_t value) {
    vm->regs[reg] = value;
    printf("MOV R%d, %u\n", reg, value);
}

// ADD指令：将两个寄存器的值相加并存入目标寄存器
void exec_ADD(VirtualMachine* vm, int dest, int src1, int src2) {
    vm->regs[dest] = vm->regs[src1] + vm->regs[src2];
    printf("ADD R%d, R%d, R%d\n", dest, src1, src2);
}

// 执行汇编程序
void execute_program(VirtualMachine* vm) {
    // 示例程序：MOV R0, 5; MOV R1, 10; ADD R2, R0, R1;
    exec_MOV(vm, 0, 5);   // R0 = 5
    exec_MOV(vm, 1, 10);  // R1 = 10
    exec_ADD(vm, 2, 0, 1); // R2 = R0 + R1 = 15
    printf("Result of R2: %d\n", vm->regs[2]);
}

int main() {
    VirtualMachine vm;
    init_vm(&vm);  // 初始化虚拟机
    execute_program(&vm);  // 执行程序
    return 0;
}
