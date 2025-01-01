arm-none-eabi-gcc -c kernel.c -o kernel.o
arm-none-eabi-gcc -c start.S -o start.o
arm-none-eabi-as -c interrupts.S -o interrupts.o
arm-none-eabi-ld -T linker.ld start.o kernel.o interrupts.o -o kernel.elf
arm-none-eabi-objcopy -O binary kernel.elf kernel.bin
qemu-system-arm -M vexpress-a9 -kernel kernel.bin -serial mon:stdio -nographic


