#include <kernel/DT/int.h>
#include <types.h>

#ifndef KERNEL_DT_KEYBOARD_H_
#define KERNEL_DT_KEYBOARD_H_

#define KB_PORT     0x60
#define KB_CMD_PORT 0x64

#define INT_KEYBOARD PIC1_INT_OFFSET + 0x01
#define INT_SERIAL   PIC1_INT_OFFSET + 0x04

#define INT_KEYBOARD_NEW_DATA 0x1
#define INT_KEYBOARD_NEW_KEY  0x2

#define KB_DATA_SELFTEST_OK    0xAA
#define KB_DATA_ECHO           0xEE
#define KB_DATA_ACK            0xFA
#define KB_DATA_SELFTEST_FAIL1 0xFC
#define KB_DATA_SELFTEST_FAIL2 0xFD
#define KB_DATA_RESEND         0xFE
#define KB_DATA_ERROR          0xFF

struct keyboard_status_t {
    uint8_t status;
    uint8_t keycode;
};

#endif
