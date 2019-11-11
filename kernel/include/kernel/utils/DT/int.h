#include <types.h>

#ifndef KERNEL_UTILS_DT_INT_H_
#define KERNEL_UTILS_DT_INT_H_

// PIC1/2 ports
#define PIC1_CMD_PORT 	0x20
#define PIC1_DATA_PORT 	0x21
#define PIC2_CMD_PORT 	0xA0
#define PIC2_DATA_PORT 	0xA1
#define KB_PORT			0x60
#define KB_CMD_PORT		0x64

// PIC initialization/... codes
#define ICW1_ICW4		0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE		0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL		0x08		/* Level triggered (edge) mode */
#define ICW1_INIT		0x10		/* Initialization - required! */
 
#define ICW4_8086		0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO		0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM		0x10		/* Special fully nested (not) */
#define PIC_EOI			0x20		/* End-of-interrupt command code */

// PIC interruption offsets
#define PIC1_INT_OFFSET 0x20
#define PIC2_INT_OFFSET 0x28
#define INT_TIMER		PIC1_INT_OFFSET + 0x00
#define INT_KEYBOARD	PIC1_INT_OFFSET + 0x01


#define PUSHAD_C uint32_t in_edi, uint32_t in_esi, uint32_t in_ebp, uint32_t in_esp, uint32_t in_ebx, uint32_t in_edx, uint32_t in_ecx, uint32_t in_eax

/* A piece of a little magic */
static inline void io_wait(void) {
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    /* %%al instead of %0 makes no difference.  TODO: does the register need to be zeroed? */
}

extern void pic_init();
extern void pic_enable();

extern void cint_division_by_zero(PUSHAD_C);
extern void cint_double_fail(PUSHAD_C);
extern void cint_segment_not_present(PUSHAD_C);
extern void cint_general_protect(uint16_t cs, PUSHAD_C, uint32_t error_code, uint32_t in_eip);
extern void cint_aligment_check(PUSHAD_C);
extern void cint_page_fault(size_t addr,PUSHAD_C);
extern void cint_timer(PUSHAD_C);
extern void cint_keyboard(PUSHAD_C);


// from asm handlers
extern void int_division_by_zero();
extern void int_double_fail();
extern void int_segment_not_present();
extern void int_general_protect();
extern void int_aligment_check();
extern void int_page_fault();
extern void int_timer();
extern void int_keyboard();

#endif
