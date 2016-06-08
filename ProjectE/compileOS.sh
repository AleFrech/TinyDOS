#!/bin/bash
#compileOS 2.0
 rm floppya.img
rm kernel
rm kernel.o
rm kernel_asm.o
echo "Files deleted!!!!!!"
echo "*********************************************"
dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
echo "*********************************************"
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
echo "*********************************************"
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
echo "*********************************************"
echo "-------Compiling Second Program--------------"
bcc -ansi -c -o shell.o shell.c
as86 -o os_api.o os_api.asm
ld86 -o shell -d shell.o os_api.o
dd if=shell of=floppya.img bs=512 count=1 seek=11 conv=notrunc
echo "****************************************"
echo "Open fuse driver"
fuse-smallfs-master/bin/sfs /home/alejandrofrech/Desktop/osproject/ProjectE/floppya.img fuse-smallfs-master/bin/sfs_root/ -s
echo "copying files..."
cp shell fuse-smallfs-master/bin/sfs_root
cp tstpr2 fuse-smallfs-master/bin/sfs_root
cp tstprg fuse-smallfs-master/bin/sfs_root
cp phello fuse-smallfs-master/bin/sfs_root
cp message.txt fuse-smallfs-master/bin/sfs_root
echo "Done!"
fusermount -u fuse-smallfs-master/bin/sfs_root
echo "Close fuse driver"
echo "Done!!!!!!!"