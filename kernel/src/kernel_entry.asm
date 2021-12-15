[bits 32]

global _start
[extern main]

_start:
    push eax
    call kernel_main

call main
jmp $

_stop:
    cli
    jmp _stop