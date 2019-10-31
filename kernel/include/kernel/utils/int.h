#include <stddef.h>
#include <stdint.h>

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
extern void int_double_fail();
extern void int_general_protect();
extern void int_aligment_check();

#endif
