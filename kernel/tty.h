#include <stddef.h>
#include <stdint.h>
#include <kernel/io.h>

#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

void terminal_initialize(void);
void terminal_clear(void);
void terminal_setcursor(uint16_t pos);
void terminal_putchar(char c);
void terminal_setcolor(uint8_t color);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
Scancode readcode(void);
void clearline(void);
void readchar(void);
char readchar(void);
char* readline(char* acc,char echo, int n);

void outportb(uint16_t port, uint8_t val);
uint8_t inportb(uint16_t port);
uint16_t inports(uint16_t _port);
void outports(uint16_t _port, uint16_t _data);
uint32_t inportl(uint16_t _port);
void outportl(uint16_t _port, uint32_t _data);


#define outportw(x,y) outports(x,y)
#define inportw(x) inports(x)

#endif