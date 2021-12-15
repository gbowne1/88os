#!/bin/bash

echo "Option Used: $1"

# Just make sure some things are setup...
export PREFIX="$HOME/opt/cross"
export TARGET=i386-elf
export PATH="$HOME/opt/cross/bin:$PATH"

# Build the Assembly Boot File
i386-elf-as boot.s -o boot.o

#Compile the Kernel in C
i386-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

#Link the Kernel
i386-elf-gcc -T linker.ld -o 88os.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

#Verify Multiboot
if grub-file --is-x86-multiboot 88os.bin; then
	echo Multiboot Compatible 
else
	echo NOT Multiboot Compatible 
	exit
fi

#Reset ISO Folder, Place Appropriate Files
rm -r 88os/
mkdir -p 88os/boot/grub
cp 88os.bin 88os/boot/88os.bin
cp grub.cfg 88os/boot/grub/grub.cfg

#Format as ISO
grub-mkrescue -o 88os.iso 88os

#Check if User Wants to Run in QEMU
if [[ $1 = R ]]; then
	echo "Running in QEMU..."
	qemu-system-i386 -cdrom 88os.iso
else
	echo "Done."
fi