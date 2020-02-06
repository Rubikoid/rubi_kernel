#include <kernel/dev/dev.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_DEV_TTY_H_
#define KERNEL_DEV_TTY_H_

extern const char *tty_dev_name;
extern char tty_output_buff[VGA_SIZE];
extern char tty_input_buff[VGA_COLS];

extern char *tty_output_ptr;
extern char *tty_input_ptr;

extern uint8_t read_line_mode;
extern uint8_t is_echo;

extern void tty_init();
extern void tty_keyboard_ih_low(uint32_t number, struct ih_low_data_t* data);
extern void tty_keyboard_ih_high(struct message_t *msg);

#endif
