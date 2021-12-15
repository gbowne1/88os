# OS Dev #

## How to build ##

Since I use both x86-64 Mac OS X as host os & GRUB to boot kernel, so I use cross gcc & binutils (for elf32-i386)

So change toolchain in `makefile` if needed

~~`mkisofs` (which is available on Mac) is used as the tool to generate iso files~~

`grub-mkrescue` is used to generate the iso file with GRUB 2 as the boot loader

`grub-mkrescue` is built for target `i386-elf` (so partly by the cross toolchain) under OS X

Just `make all` to build the final `os.iso`

## Running ##

~~`bochs` is used as a x86 simulator and debugger, a default `.bochsrc` is provided~~

Switch to `qemu` to enable debugging with `gdb`

A `.gdbinit` is provided to configure & link `gdb` with `qemu`

~~The boot loader is GRUB 0.97, using `stage2_eltorito` for ISO~~

~~Both the GRUB binary and `menu.lst` are provided~~

~~Use `make run` to start the simulator if `bochs` is installed~~

The boot loader is GRUB 2, a default `grub.cfg` is provided

Use `make qemu` to start the simulator __in background__

Use `make run` to start both the simulator and `gdb`, which the simulator will then stop at BIOS boot loader

In `gdb`, customer function `re` is defined to restart the simulator when __it is killed__

Annoyingly, `Ctrl-C` (Signal 2) will kill the background simulator in `gdb` and a solution has not been found on OS X

## Progress ##

### Framework ###

- [x] Bare Bones
- [x] Higher Address Kernel
- [x] Switch to `qemu` & `gdb`
- [x] Upgrade to GRUB 2
- [ ] ELF Loader

### Memory ###

- [x] Segmentation
- [x] Kernel Object Manager
- [ ] Frame Allocator
- [ ] User Mode Paging

### IO ###

- [x] Debug Device COM1
- [x] Generic Device Manager
- [x] Framebuffer
- [x] Generic PCI
- [x] ATA/ATAPI Disk Detect
- [x] ATAPI Disk Read
- [ ] ATA Disk Read/Write

### Interrupt Requests ###

- [x] Interrupt
- [x] PIC
- [x] Real Time Clock

### Filesystem ###

- [x] Virtual Filesystem Framework
- [x] RootFS
- [x] DevFS
- [ ] Block Device & Buffer Cache
- [ ] ISO-9660

### Misc ###

- [x] `multiboot` Info
- [x] Generic Double-linked List
- [ ] Basic String Operations & Format

## Author ##



## License ##

GNU

## Reference ##

- [《x86汇编语言：从实模式到保护模式》](https://www.amazon.cn/dp/B00AR0ZSVO/)
- [『30日でできる! OS自作入門』](https://www.amazon.co.jp/dp/4839919844)
- [The little book about OS development](https://littleosbook.github.io/)
- [OSDev.org](http://www.osdev.org/)
