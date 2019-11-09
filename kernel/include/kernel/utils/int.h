#include <types.h>

#ifndef KERNEL_UTILS_INT_H_
#define KERNEL_UTILS_INT_H_

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1
#define PIC_IQW_CMD 0x11

extern void pic_init();
extern void pic_enable();

extern void abort(char *death_message);


extern void cint_division_by_zero();
extern void cint_double_fail();
extern void cint_segment_not_present();
extern void cint_general_protect();
extern void cint_aligment_check();
extern void cint_page_fault();

#endif
