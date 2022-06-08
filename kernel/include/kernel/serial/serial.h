#include <kernel/defines.h>
#include <types.h>

#ifndef KERNEL_SERIAL_SERIAL_H_
#define KERNEL_SERIAL_SERIAL_H_

#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8
#define COM3_PORT 0x3E8
#define COM4_PORT 0x2E8

#define COM_DATA 0
#define COM_INT  1

#define COM_D_LSB_DIV 0
#define COM_D_MSB_DIV 1

#define COM_INT_FIFO   2
#define COM_D_MSB      3
#define COM_MODEM_CTRL 4
#define COM_LINE_STAT  5
#define COM_MODEM_STAT 6
#define COM_SCATCH     7

extern void init_com(uint16_t com_id);
extern void write_com(uint16_t com_id, uint8_t data);
extern uint8_t read_com(uint16_t com_id, uint8_t blocking);

#endif
