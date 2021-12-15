#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdint.h>
#define SHIFT 0x2A
typedef struct
{
  unsigned char Shift:1;
  unsigned char Alt:1;
  unsigned char Super:1;
  unsigned char Control:1;
  unsigned char unused:4;
  char code;
} Scancode;
struct pci_device{unsigned char bus;unsigned char device;unsigned short class;unsigned short vendor;unsigned char function;};
const char scancode[256];
const char scancode_shift[256];
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
Scancode getScancode(void);
char ask_cmos(char reg);
void reboot (void);
void outl(uint16_t port, unsigned int val);
unsigned int inl(uint16_t port);
uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,uint8_t func, uint8_t offset);
unsigned int pciConfigReadReg(uint8_t bus, uint8_t slot,uint8_t func, uint8_t offset);
void pciEnumAll();
struct pci_device devices[32];
int n_devices;

/** outb:
 *  Sends the given byte to the given I/O port.
 *
 *  @param port The I/O port to send the data
 *  @param data The data to send to the I/O port (in byte)
 */

void outb(unsigned short port, unsigned char data);

/** inb:
 *  Reads byte from the given I/O port.
 *  @param port The I/O port to read the data
 */

unsigned char inb(unsigned short port);

#endif