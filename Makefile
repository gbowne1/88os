version = 0.0.1
arch ?=x86_32

#names
kernel_fullname: := kernel-$(arch).bin
iso := build/kernel-$(version)-$(arch).bin

#include  paths
dirs = $(shell find src/$arch/kernel -type d -print)
includedirs := $(sort $(foreach dir, $foreach dir1, $(dirs), $(shell dirname $(dir1))), $(wildcard $(dir)/include)))))

CC = gcc
AS = nasm
GCC=GCC
LD = ld
VPATH=src:$(SOURCE)
NASM=nasm
ASFLAGS = -f elf -march=i486
KERNEL = ./sys/kernel.bin
INITRD = build/iso/boot/initrd.img
BUILDROOT = $(shell realpath build/initrd)
TOOLROOT = $(shell realpath build/toolroot)
ISO_IMAGE = build/os.iso
PATH := "$(TOOLROOT)/bin:$(PATH)"
LDSCRIPT = $(SRCPATH)/link.ld
LDFLAGS = -T $(LDSCRIPT) -melf_i386
ISO = grub-mkrescue
ISOFLAGS =
GRUB = grub.cfg
GRUBFILE = $(patsubst %, $(GRUBPATH)/%, $(GRUB))
OBJ = ${C_SOURCES:.c=.o}
C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)

.PHONY: userland clean ports
.PHONY: all
all: kernel.o

kernel: link.ld $(OBJECTS)


LIB = io/port io/serial io/device io/framebuffer io/pci io/ata \
	  mm/segment mm/kmem \
	  irq/interrupt irq/pic irq/rtc \
	  fs/vnode fs/dentry fs/superblock fs/fs fs/file \
	  fs/rootfs/rootfs fs/ramfs/ramfs fs/devfs/devfs \
	  util/string util/log util/list
OBJ = loader kmain mm/segment_s irq/interrupt_s mm/page_s \
 	  $(LIB)
LIBFILE = $(patsubst %, $(OBJPATH)/%.o, $(LIB))
LIBHEADER = $(patsubst %, $(SRCPATH)/%.h, $(LIB))
OBJFILE = $(patsubst %, $(OBJPATH)/%.o, $(OBJ))
TARGET = $(TARPATH)/kernel
IMAGE = os.iso

BOCHS = bochs
BOCHSRC = .bochsrc
BOCHSFLAGS = -f $(BOCHSRC) -q

QEMU = qemu-system-i386
QEMULOG = tmp/qemu.log
QEMUSERIAL = tmp/com1.log
QEMUFLAGS = -D $(QEMULOG) -serial file:$(QEMUSERIAL) -s -S \
			-drive file=tmp/hdd.vmdk,if=ide,index=0,media=disk,format=vmdk \
 			-drive file=$(IMAGE),if=ide,index=1,media=cdrom

GDB = i386-elf-gdb

all : $(IMAGE)

run : all
	$(GDB)

qemu: all $(TMPPATH)
	$(QEMU) $(QEMUFLAGS) &

rebuild : clean all

clean :
	rm -fr $(IMAGE) $(TARGET) $(OBJPATH)/*

$(TMPPATH) :
	mkdir -p $@

$(IMAGE) : $(TARGET) $(GRUBFILE) makefile
	$(ISO) $(ISOFLAGS) -o $(IMAGE) $(TARPATH)

$(TARGET) : $(OBJFILE) $(LDSCRIPT) makefile
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJFILE)

$(GRUBPATH)/% : $(SRCPATH)/% makefile
	mkdir -p $(dir $@)
	cp $< $@

$(OBJPATH)/%.o : $(SRCPATH)/%.s makefile
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(OBJPATH)/%.o : $(SRCPATH)/%.c makefile
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJPATH)/kmain.o : $(LIBHEADER)

$(LIBFILE) : $(OBJPATH)/%.o : $(SRCPATH)/%.h

.PHONY : all clean rebuild

#linker
linker_script := src/$(arch)/kernel/link.ld
LFLAGS := -nostdlib -z max-page-size=0x1000	-m elf_i386

#flags
CFLAGS= -m32 -fno-pie -fno-builtin -nostdinc -nostdlib -nostdinc -ffreestanding -fno-stack-protector -g -Wall -Wextra \
               -Werror -c -I. -MMD -mno-red-zone -Wl,--build-id=none --target=x86_64-elf
# Wunused-variable will be ignored!

#automatically include any header in dirs called include
CFLAGS += $(foreach dir, $(includedirs), -I./$(dir))

#assembly
ASMFLAGS = -m32 -nostdlib
ASMFLAGS += $(foreach dir, $(includedirs), -I./$(dir))

NASMFLAGS = -felf32

#support for .S
assembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.S))
assembly_object_files := $(patsubst src/$(arch)/kernel/%.S, \
	build/$(arch)/kernel/%.o, $(assembly_source_files))

#support for .asm
nassembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.asm))
nassembly_object_files := $(patsubst src/$(arch)/kernel/%.asm, \
	build/$(arch)/kernel/%.o, $(nassembly_source_files))


c_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.c))
c_object_files := $(patsubst src/$(arch)/kernel/%.c, \
    build/$(arch)/kernel/%.o, $(c_source_files))
#qemu

qemumem := 1
qemuflags := -device isa-debug-exit,iobase=0xf4,iosize=0x04 -serial stdio

.PHONY: all clean run runrel iso

all: toolchain kernel userland-libraries userland iso

install:
	@sudo apt-get install qemu nasm xorriso grub2

clean:
	@rm -r -f $ISO_IMAGE) $(KERNEL) $(INITRD) build

iso: $(KERNEL) $(INITRD) $(ISO_IMAGE)

run: $(iso)
	@echo starting emulator...
	qemu-system-x86_64 -cdrom $(iso) -m $(qemumem)G $(qemuflags)

run-wsl: $(iso)
	@echo starting emulator...
	qemu-system-x86_64 -cdrom $(iso) -m $(qemumem)G $(qemuflags) -display curses

$(iso): $(kernel)
	@./scripts/generate_iso.sh $(kernel) $(iso)


$(kernel): $(assembly_object_files) $(c_object_files) $(nassembly_object_files) $(linker_script)
	@echo linking...
	@ld $(LFLAGS) -T $(linker_script) -o $(kernel) $(assembly_object_files) $(nassembly_object_files) $(c_object_files)


# compile assembly files
build/$(arch)/kernel/%.o: src/$(arch)/kernel/%.S
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@gcc -c $(ASMFLAGS) $< -o $@

# compile assembly files
build/$(arch)/kernel/%.o: src/$(arch)/kernel/%.asm
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@nasm $(NASMFLAGS) $< -o $@

# compile c files
build/$(arch)/kernel/%.o: src/$(arch)/kernel/%.c
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@gcc -c $(CFLAGS) $< -o $@

boot/boot_sect.bin: boot/boot_sect.asm
		nasm $< -f bin -o $@

CFLAGS = -g -m32 -fno-stack-protector -fno-builtin -fnostack-protector -fno-asynchronous-unwind-tables \
         -nostdlib -nostdinc -fno-PIC -fno-PIE -Wall -Wextra -Werror -I.
ASFLAGS = -felf32
LDFLAGS = -z max-page-size=0x1000 -melf_i386 -T linker.ld

GPPPARAMS_KERNEL = -m32 -Ikernel/include -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fcheck-new

OBJECTS=loader.o
asm_interrupts.o \
assembly_interface.o \
data_structures/global_descriptor_table.o \
data_structures/interrupt_descriptor_table.o \
data_structures/page_table.o \
data_structures/symbol_table.o \
drivers/frame_buffer.o \
drivers/keyboard.o \
drivers/pic.o \
drivers/serial_port.o \
interrupts.o \
kernel_filesystem.o \
kernel_stdio.o \
kernel_syscalls.o \
kmain.o \
multiboot_utils.o \
process.o \
stdlib.o

STDLIB = stdlib/stdio.o \
stdlib/string.o \
stdlib/syscalls.o

device = 

boot_files = out/bin/bootloader1.bin \
            out/bin/bootloader2.bin

kernel_files = out/obj/kernel/kernel_entry.o \
				out/obj/kernel/util/screen.o \
				out/obj/kernel/util/system.o \
				out/obj/kernel/util/events.o \
				out/obj/kernel/util/number.o \
				out/obj/kernel/util/string.o \
				out/obj/kernel/util/images/bitmap.o \
				out/obj/kernel/IO/port.o \
				out/obj/kernel/IO/interrupts.o \
				out/obj/kernel/IO/interruptstubs.o \
				out/obj/kernel/IO/pci.o \
				out/obj/kernel/drivers/driver.o \
				out/obj/kernel/drivers/keyboard.o \
				out/obj/kernel/drivers/ide.o \
				out/obj/kernel/drivers/ahci.o \
				out/obj/kernel/drivers/ehci.o \
				out/obj/kernel/drivers/usb.o \
				out/obj/kernel/drivers/usb_mass_storages.o \
				out/obj/kernel/drivers/filesystem.o \
				out/obj/kernel/graphics/desktop.o \
				out/obj/kernel/graphics/widget.o \
				out/obj/kernel/graphics/window.o \
				out/obj/kernel/memory_manager.o \
				out/obj/kernel/multitasking.o \
				out/obj/kernel/kernel_main.o

out/bin/%.bin: bootloader/%.asm
	mkdir -p out
	mkdir -p out/bin
	nasm -f bin $< -o $@

out/obj/kernel/%.o: kernel/src/%.asm
	mkdir -p out
	mkdir -p out/obj
	mkdir -p out/obj/kernel
	nasm -f elf32 $< -o $@

out/obj/kernel/%.o: kernel/src/%.cpp
	mkdir -p out
	mkdir -p out/obj
	mkdir -p out/obj/kernel
	mkdir -p out/obj/kernel/util
	mkdir -p out/obj/kernel/util/images
	mkdir -p out/obj/kernel/IO
	mkdir -p out/obj/kernel/drivers
	mkdir -p out/obj/kernel/graphics
	i586-elf-g++ $(GPPPARAMS_KERNEL) -c $< -o $@

kernel.bin: $(kernel_files)
	mkdir -p out
	mkdir -p out/bin
	i586-elf-ld -Tkernel/linker.ld -o out/bin/kernel.bin $(kernel_files)

install: $(boot_files) kernel.bin
	dd if=out/bin/bootloader1.bin of=$(device)
	dd if=out/bin/bootloader2.bin of=$(device) seek=1
	dd if=out/bin/kernel.bin of=$(device) seek=3

run: install
	VirtualBox --startvm "CrystalOS"

.PHONY: clean
clean:
	rm -rf out
	rm -f *.iso
	rm -f $(OBJECTS)
	rm -f *.elf # elf executables
	rm -f *.bin # flat binary executables
	rm -f *.out
	rm -rf iso/
	rm -f built_file_system