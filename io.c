#include <io.h>
#include <stdint.h>

void outportb(uint16_t port, uint8_t val) {
    asm volatile("outb %1, %0" : : "dN"(port), "a"(val));
}

/*
 * read a byte
 * */
uint8_t inportb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/*
 * Read 2 bytes
 * */
uint16_t inports(uint16_t _port) {
    uint16_t rv;
    asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/*
 * Write 2 bytes
 * */
void outports(uint16_t _port, uint16_t _data) {
    asm volatile ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

/*
 * Read 4 bytes
 * */
uint32_t inportl(uint16_t _port) {
    uint32_t rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (_port));
    return rv;
}

/*
 * Write 4 bytes
 * */

void outportl(uint16_t _port, uint32_t _data) {
    asm volatile ("outl %%eax, %%dx" : : "dN" (_port), "a" (_data));
}
size_t sh_write(int fd, const void* buffer, size_t byte){
  return system_call(SYSCALL_WRITE, fd, buffer, byte);
}

size_t sh_read(int fd, void* buffer, size_t byte){
  return system_call(SYSCALL_READ, fd, buffer, byte);
}