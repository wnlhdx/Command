#!/bin/bash
set -e

arm-none-eabi-as -o start.o start.S
arm-none-eabi-gcc -c kernel.c -o kernel.o -nostdlib -ffreestanding
arm-none-eabi-ld -T linker.ld -o kernel.elf start.o kernel.o
arm-none-eabi-objcopy -O binary kernel.elf kernel.bin
