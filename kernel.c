#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

#inlcude "<stdio.h>"
#include "../cpu/isr.h"
#include "../cpu/paging.h"
#include "../cpu/ports.h"
#include "../drivers/acpi.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include <signal.h>
#include <kernel/tty.h>
#include <kernel/kernel.h>
#include <common.h>
#include <idt.h>
#include <terminal.h>
#include <terminal_backend_b8000.h>
#include "system.h"
#include "system.c"
#include "string.c"
#include "screen.c"

/*void kernel_main(void) {
    terminal_initialize();

    show_logo();

    printf("\n\n\n\n\n\n\n\n");
    printf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
}*/

void kernel_main(uint32_t addr) {
    // Point to Multiboot info table and clear screen
    init_video(addr);
    // Set IDT gates, remap the PIC and install the IRQs
    isr_install();
    /* Enable interrupts, set the timer interrupt (IRQ0)
     * and the keyboard interrupt (IRQ1) */
    irq_install();
    // Map ACPI tables and send the enable command
    acpi_install();
    // Enable paging and identity map kernel and video memory
    init_paging();
    // Map every possible signal
    init_signal();
    // Test serial port (COM1)
    serial_install();

    kprint("Boot success.\n"
           "\n");

    kprint("88os comes with ABSOLUTELY NO WARRANTY,\n"
           "to the extent permitted by applicable law.\n");

    kprint("\nType 'help' to get started\n> ");
}

void show_logo(void) {
    terminal_clear();
    printf("\n\n\n\n\n\n\n\n");
    terminal_setcolor(10); // 10 = lightgreen
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    terminal_setcolor(15); // white
}

kernel_main(){
	initialize_console();
	puts("Hello, World!\n");
	set_text_color(RED, LIGHT_GREY);
	puts("\tWelcome to my OS!\n");
	set_text_color(WHITE, BLACK);
	puts("The address for video memory is: ");
	puthex(0xb8000);
	for(;;);
}

int div(int a, int b) {
    return a/b;
}

void _start(void* kernel_location) {
    UNUSED(kernel_location);
    SetIDTR();
}

void _start(void* kernel_location) {
  UNUSED(kernel_location);
  SetIDTR();

  TerminalBackend *con = TerminalBackendB8000();

  T_ClearScreen(con);
  T_PutText(con, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\t\tXXXX\n");
  T_Printf(con, "%p %x %i %s %c %u\n",
      con, 0x41424344, -1234, "alamakota", 'X', 1234567890123LL);

  //unsigned long long addr = (unsigned long long)kernel_location;
  //unsigned long long addr = (unsigned long long)k;
  //for (int i = 0; i < 16; i++) {
  //  textvram[i * 2] = "0123456789ABCDEF"[(addr >> 60) & 0xf];
  //  addr <<= 4;
  //}

  int a = 0, b = 0;
  __asm__ volatile("div %2\n"
       : "=d" (a), "=a" (b)
       : "r" (a), "d" (a), "a" (a));

  T_PutText(con, "It came back!");

  for(;;);

void main() {
	char* video_memory = (char *) 0xb8000;
	*video_memory = 'X';
}