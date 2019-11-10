#include <types.h>

#ifndef KERNEL_UTILS_DT_INT_H_
#define KERNEL_UTILS_DT_INT_H_

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1
#define PIC_IQW_CMD 0x11

#define PUSHAD_C uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t esp, uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax

extern void pic_init();
extern void pic_enable();

extern void cint_division_by_zero(PUSHAD_C);
extern void cint_double_fail(PUSHAD_C);
extern void cint_segment_not_present(PUSHAD_C);
extern void cint_general_protect(PUSHAD_C);
extern void cint_aligment_check(PUSHAD_C);
extern void cint_page_fault(size_t addr,PUSHAD_C);

// from asm handlers
extern void int_division_by_zero();
extern void int_double_fail();
extern void int_segment_not_present();
extern void int_general_protect();
extern void int_aligment_check();
extern void int_page_fault();

#endif
