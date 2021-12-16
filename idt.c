/* File: idt.c
 *
 * Implements Interrupt Descriptor Table construction / modification
 */
#include <system.h>
#include <idt.h>

/* Pointer to gate descriptor in the IDT */
static struct idt_gate_descriptor* idt_ptr
= (struct idt_gate_descriptor*)IDT_BASE;

/* IDT Register struct */
static struct idtr_struct idtr_info
= { .length = 0, .address = IDT_BASE };

/* Pointer to wrapper function */
static struct isr_wrapper_struct* isr_wrapper_ptr
= (struct isr_wrapper_struct*)ISR_WRAPPER_BASE;

/* Generic ISR wrapper function */
static struct isr_wrapper_struct generic_wrapper
= { PUSHAD_OPCODE, CLD_OPCODE, CALL_FAR_OPCODE,
    0, 0x08, POPAD_OPCODE, IRET_OPCODE };

/**
 * __set_isr_wrapper() - Initialize a new ISR wrapper struct.
 * @wrapper_ptr: pointer to wrapper struct to write
 * @isr_addr:    address of ISR function
 */
static inline void __set_isr_wrapper (struct isr_wrapper_struct* wrapper_ptr,
				      uint32_t isr_addr)
{
  generic_wrapper.isr_addr = isr_addr;
  *wrapper_ptr = generic_wrapper;
}

/**
 * __set_idt_gate() - Assign parameters to a gate descriptor.
 * @gate:      pointer to gate struct
 * @segment:   segment selector
 * @type_attr: type/attribute byte
 */
static void __set_idt_gate (struct idt_gate_descriptor* gate,
			    struct isr_wrapper_struct* isr_wrapper_offset,
			    uint16_t segment, uint8_t type_attr)
{
  uint32_t tmp_offset;

  tmp_offset = (uint32_t)isr_wrapper_offset;
  tmp_offset <<= 16;
  tmp_offset >>= 16;
  gate->offset_low = (uint16_t)tmp_offset; /* low 16 bits */

  tmp_offset = (uint32_t)isr_wrapper_offset;
  tmp_offset >>= 16;
  gate->offset_high = (uint16_t)tmp_offset; /* high 16 bits */

  gate->segment_selector = segment;
  gate->zero = 0;
  gate->type_attr = type_attr;
}

static inline void add_null_gates (int interrupt_number, int* idt_gate_count)
{
  while (interrupt_number - *idt_gate_count)
    {
      __set_idt_gate(idt_ptr + *idt_gate_count,
		     isr_wrapper_ptr + interrupt_number, 0, 0);

      __set_isr_wrapper(isr_wrapper_ptr + interrupt_number, 0);

      (*idt_gate_count)++;
    }
}

/**
 * idt_install_interrupt() - Make a new IDT gate and add it to the IDT.
 * @interrupt_number: Interrupt index into IDT
 * @isr:              pointer to ISR function
 * @type_attr:        type/attribute byte
*/
void idt_install_interrupt (int interrupt_number,
			    void (*isr)(void), uint8_t type_attr)
{
  static int idt_gate_count = 0;

  // index is determined by IBM PC standard
  if (idt_gate_count <= interrupt_number) /* if (new addition to IDT) */
    {
    // adds empty idt gates between the current last idt gate in the array
    // and the new idt gate we are adding iff the index of the new idt gate is
    // more than one greater than the index of the current last idt gate

      add_null_gates(interrupt_number, &idt_gate_count);
      idt_gate_count++;
    }

  // adds a new idt gate for the new isr to the array of idt gates
  // TODO: replace magic number
  __set_idt_gate(idt_ptr + interrupt_number,
		 isr_wrapper_ptr + interrupt_number, 0x08, type_attr);

  // adds a new isr wrapper for the new isr to the array of isr wrappers
  __set_isr_wrapper(isr_wrapper_ptr + interrupt_number, (uint32_t)isr);

  idtr_info.length = 8 * idt_gate_count - 1;
}

struct idtr_struct idt_store ()
{
  struct idtr_struct idt_old;

  asm(".intel_syntax noprefix\n"

      "sidt [esp]\n"

      ".att_syntax\n");

  return idt_old;
}

// load the idt register with the size and address of the interrupt
// descriptor table for the CPU
void idt_load ()
{
  asm(".intel_syntax noprefix\n"

      "lidt [idtr_info]\n"

      ".att_syntax\n");

}

typedef struct idt_entry
{
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__ ((packed)) idt_entry_t;

typedef struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed)) idt_ptr_t;

idt_entry_t idt[256];
idt_ptr_t idtp;

extern void idt_load();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].always0 = 0;
    idt[num].sel = sel;
    idt[num].flags = flags;
}

void idt_install()
{
    idtp.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtp.base = &idt;

    memset(&idt, 0, sizeof(idt_entry_t) * 256);

    idt_load();
}

/* Pointer to gate descriptor in the IDT */
static struct idt_gate_descriptor* idt_ptr
= (struct idt_gate_descriptor*)IDT_BASE;

/* IDT Register struct */
static struct idtr_struct idtr_info
= { .length = 0, .address = IDT_BASE };

/* Pointer to wrapper function */
static struct isr_wrapper_struct* isr_wrapper_ptr
= (struct isr_wrapper_struct*)ISR_WRAPPER_BASE;

/* Generic ISR wrapper function */
static struct isr_wrapper_struct generic_wrapper
= { PUSHAD_OPCODE, CLD_OPCODE, CALL_FAR_OPCODE,
    0, 0x08, POPAD_OPCODE, IRET_OPCODE };

/**
 * __set_isr_wrapper() - Initialize a new ISR wrapper struct.
 * @wrapper_ptr: pointer to wrapper struct to write
 * @isr_addr:    address of ISR function
 */
static inline void __set_isr_wrapper (struct isr_wrapper_struct* wrapper_ptr,
				      uint32_t isr_addr)
{
  generic_wrapper.isr_addr = isr_addr;
  *wrapper_ptr = generic_wrapper;
}

/**
 * __set_idt_gate() - Assign parameters to a gate descriptor.
 * @gate:      pointer to gate struct
 * @segment:   segment selector
 * @type_attr: type/attribute byte
 */
static void __set_idt_gate (struct idt_gate_descriptor* gate,
			    struct isr_wrapper_struct* isr_wrapper_offset,
			    uint16_t segment, uint8_t type_attr)
{
  uint32_t tmp_offset;

  tmp_offset = (uint32_t)isr_wrapper_offset;
  tmp_offset <<= 16;
  tmp_offset >>= 16;
  gate->offset_low = (uint16_t)tmp_offset; /* low 16 bits */

  tmp_offset = (uint32_t)isr_wrapper_offset;
  tmp_offset >>= 16;
  gate->offset_high = (uint16_t)tmp_offset; /* high 16 bits */

  gate->segment_selector = segment;
  gate->zero = 0;
  gate->type_attr = type_attr;
}

static inline void add_null_gates (int interrupt_number, int* idt_gate_count)
{
  while (interrupt_number - *idt_gate_count)
    {
      __set_idt_gate(idt_ptr + *idt_gate_count,
		     isr_wrapper_ptr + interrupt_number, 0, 0);

      __set_isr_wrapper(isr_wrapper_ptr + interrupt_number, 0);

      (*idt_gate_count)++;
    }
}

/**
 * idt_install_interrupt() - Make a new IDT gate and add it to the IDT.
 * @interrupt_number: Interrupt index into IDT
 * @isr:              pointer to ISR function
 * @type_attr:        type/attribute byte
*/
void idt_install_interrupt (int interrupt_number,
			    void (*isr)(void), uint8_t type_attr)
{
  static int idt_gate_count = 0;

  // index is determined by IBM PC standard
  if (idt_gate_count <= interrupt_number) /* if (new addition to IDT) */
    {
    // adds empty idt gates between the current last idt gate in the array
    // and the new idt gate we are adding iff the index of the new idt gate is
    // more than one greater than the index of the current last idt gate

      add_null_gates(interrupt_number, &idt_gate_count);
      idt_gate_count++;
    }

  // adds a new idt gate for the new isr to the array of idt gates
  // TODO: replace magic number
  __set_idt_gate(idt_ptr + interrupt_number,
		 isr_wrapper_ptr + interrupt_number, 0x08, type_attr);

  // adds a new isr wrapper for the new isr to the array of isr wrappers
  __set_isr_wrapper(isr_wrapper_ptr + interrupt_number, (uint32_t)isr);

  idtr_info.length = 8 * idt_gate_count - 1;
}

struct idtr_struct idt_store ()
{
  struct idtr_struct idt_old;

  asm(".intel_syntax noprefix\n"

      "sidt [esp]\n"

      ".att_syntax\n");

  return idt_old;
}

// load the idt register with the size and address of the interrupt
// descriptor table for the CPU
void idt_load ()
{
  asm(".intel_syntax noprefix\n"

      "lidt [idtr_info]\n"

      ".att_syntax\n");

}