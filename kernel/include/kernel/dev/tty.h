#include <kernel/dev/dev.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_DEV_TTY_H_
#define KERNEL_DEV_TTY_H_

#define TTY_IOCTL_CLEAR 		 		(1 << 8)
#define TTY_IOCTL_READ_MODE_LINE_ON 	(2 << 8)
#define TTY_IOCTL_READ_MODE_ECHO_ON 	(3 << 8)
#define TTY_IOCTL_READ_MODE_LINE_OFF 	(4 << 8)
#define TTY_IOCTL_READ_MODE_ECHO_OFF 	(5 << 8)
#define KEYBOARD_MAP_SIZE 128

extern unsigned char keyboard_map[KEYBOARD_MAP_SIZE];

extern const char *tty_dev_name;
extern char tty_output_buff[1];
extern char tty_input_buff[VGA_COLS];

extern char *tty_output_ptr;
extern char *tty_input_ptr;

extern uint8_t read_line_mode;
extern uint8_t is_echo;

extern void tty_init();
extern void tty_keyboard_ih_low(uint32_t number, struct ih_low_data_t* data);
extern void tty_keyboard_ih_high(struct message_t *msg);

extern void tty_ioctl(struct io_buf_t* io_buf, uint32_t command);

extern void tty_write(struct io_buf_t* io_buf, void* data, uint32_t size);
extern void tty_write_ch(struct io_buf_t* io_buf, char ch);

extern char tty_read_ch(FILE* io_buf);
extern uint32_t tty_read(FILE* io_buf, void *buffer, uint32_t size);


#endif
