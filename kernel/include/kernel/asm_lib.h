#include <types.h>

#ifndef KERNEL_ASMLIB_H_
#define KERNEL_ASMLIB_H_

extern void disable_int();
extern void enable_int();
extern void halt();

extern void out_byte(uint16_t port, uint8_t value);
extern uint8_t in_byte(uint16_t port);

extern void gdt_load(void *gdt_ptr);
extern void idt_load(unsigned long *addr);

#endif
