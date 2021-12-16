/* File: idt.h
 *
 * Interrupt Descriptor Table structures and function prototypes
 */

#include <stdint.h>
#include "asm.h"

#ifndef IDT_H
#define IDT_H

#define IDT_BASE            0x6c00 /* hardcoded address for IDT */
#define ISR_WRAPPER_BASE    0x7400 /* 0x6c00 + 0x800 (8 * 256 IDT gates) */

struct idt_gate_descriptor
{
  uint16_t offset_low;
  uint16_t segment_selector;
  uint8_t  zero;
  uint8_t  type_attr;
  uint16_t offset_high;

}__attribute__((packed, aligned(1)));

struct idtr_struct
{
  uint16_t length;
  uint32_t address;

}__attribute__((packed, aligned(1)));

// the isr wrapper struct provides additional assembly instructions that
// need to be executed before and after the interrupt handler itself --
// the address of which is designated by isr_addr and segment
struct isr_wrapper_struct
{
  uint8_t  pushad;
  uint8_t  cld;
  uint8_t  call_far;
  uint32_t isr_addr;
  uint16_t segment;
  uint8_t  popad;
  uint8_t  iretd;

}__attribute__((packed, aligned(1)));


void idt_install_interrupt (int interrupt_number,
			    void (*isr)(void), uint8_t type_attr);

struct idtr_struct idt_store ();
void idt_load ();


#endif	/* IDT_H */