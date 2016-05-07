#!/bin/bash
#compileOS 2.0
 
rm floppya.img
rm kernel
rm kernel.o
rm kernel_asm.o
echo "Files deleted!!!!!!"
dd if=/dev/zero of=floppya.img bs=512 count=2880
echo "*********************************************"
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
echo "*********************************************"
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
echo "*********************************************"
echo "Done!!!!!!!"
