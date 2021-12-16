#pragma once

#include "interrupt.h"
namespace kernel{

#define KERNEL_PAGEDIR        0x0
#define KERNEL_PAGETAB        0x1000
#define KERNEL_GDT            0x2000
#define KERNEL_IDT            0x2500
#define PIC1 0x20
#define PIC2 0xA0
#define ICW1 0x11
#define ICW4 0x01

#define KBD_SPECIAL (char)200
#define ENTER (char)10
#define F1 (char)201
#define F2 (char)202
#define F3 (char)203
#define F4 (char)204
#define F5 (char)205
#define F6 (char)206
#define F7 (char)207
#define F8 (char)208
#define F9 (char)209
#define F10 (char)210
#define F11 (char)211
#define F12 (char)212
#define PAUSE (char)213

class Keyboard{
public:
	void initialize();

};

static void kb_handler(Registers reg);
}