# OS Dev #

This is my attempt at building an x86 IA32 Operating System for i386/i486 only complete with GUI.

## How to build ##

## Running ##

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

Gregory K. Bowne

## License ##

GNU GPL v3

## Reference ##

- [The little book about OS development](https://littleosbook.github.io/)
- [OSDev.org](http://www.osdev.org/)
