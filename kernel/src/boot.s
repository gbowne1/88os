.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.set KERNEL_VBASE, 0xC0000000
.set KERNEL_PNUM, (KERNEL_VBASE >> 22)


.set STACKSIZE, 0x4000

.section .data
.balign 0x1000

_boot_pd:
  .long 0x00000083
  .fill (KERNEL_PNUM - 1), 4, 0x00000000

  .long 0x00000083 
  .fill (1024 - KERNEL_PNUM - 1), 4, 0x00000000

.section .multiboot
.balign 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack
stack_bottom:
.skip 32768 # 16 KiB
stack_top:

section .text

.global __idt_default_handler
.type __idt_default_handler, @function
__idt_default_handler:
	pushal
	mov $0x20, %al
	mov $0x20, %dx
	out %al, %dx
	#call _test
	popal
	iretl

.global _set_gdtr
.type _set_gdtr, @function
_set_gdtr:
	push %ebp
	movl %esp, %ebp

	lgdt 0x800

	movl %ebp, %esp
	pop %ebp
	ret

.global _set_idtr
.type _set_idtr, @function
_set_idtr:
	push %ebp
	movl %esp, %ebp

	lidt 0x10F0

	movl %ebp, %esp
	pop %ebp
	ret

.global _reload_segments
.type _reload_segments, @function
_reload_segments:
	push %ebp
	movl %esp, %ebp

	push %eax
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	pop %eax

	ljmp $0x8, $me
me:
	movl %ebp, %esp
	pop %ebp
	ret

.global _start # ez exportalja a _start label-t
.type _start, @function # a _start label mostmar egy funkcio!
_start: # definialjuk a _start -ot

	movl $stack_top, %esp # gyors allitsuk be a stacket!
	mov $0x1337, %eax

	call kernel_main # es hivjuk meg a C99 resz! :)

	cli # kikapcsoljuk az interruptokat
	hlt # halt a kovetkezo interruptig
	# az elozo ket sor, orokre lefagyasztja a gepet, innen marcsak a reboot segit!
.Lhang:  # ha GPF vagy PF lenne, akkor sem hagyjuk a gepet elfutni a rossz memoriaba!
	jmp .Lhang # inkabb ugraljon orokre itt (~spinlock)

.section .text
.global _loader

_loader:
  movl  $(_boot_pd - KERNEL_VBASE), %ecx
  movl %ecx, %cr3

  // pse bit set
  movl %cr4, %ecx
  orl $0x00000010, %ecx
  movl %ecx, %cr4

  // enable paging
  movl %cr0, %ecx
  orl $0x80000000, %ecx
  movl %ecx, %cr0

  leal (_start), %ecx
  jmp *%ecx

.type _start, @function
_start:
  movl $0, (_boot_pd)
  
  // TLB Flush
  invlpg (0) 
  
  movl $stack_top, %esp
  addl $KERNEL_VBASE, %ebx
  pushl %eax
  pushl %ebx
  
  call kernel_main
  cli

1: hlt
  jmp 1b

.size _start, . - _start

.section .bss
.balign 32
stack_bottom:
.skip STACKSIZE
stack_top: