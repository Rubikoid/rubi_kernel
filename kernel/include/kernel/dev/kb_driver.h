#include <kernel/dev/dev.h>
#include <kernel/scheduler/task.h>
#include <lib/clist.h>
#include <lib/queue.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_DEV_KEYBOARD_H_
#define KERNEL_DEV_KEYBOARD_H_

#define KEYBOARD_IOCTL_ECHO (1 << 8)

#define KB_STATUS_IDLE     0x0  // just idle, after end of cmd exec
#define KB_STATUS_RESEND0  0x1
#define KB_STATUS_RESEND1  0x2
#define KB_STATUS_RESEND2  0x3  // we should resend command 3 times or agree with keyboard stupidity
#define KB_STATUS_FAILED   0x4
#define KB_STATUS_IN_WORK0 0x5
#define KB_STATUS_IN_WORK1 0x6
#define KB_STATUS_IN_WORK2 0x7
#define KB_STATUS_IN_WORK3 0x8  // during any command execution save state.

typedef uint8_t (*sendbyte_func)(struct keyboard_status_t* status);

struct kb_queue_t {
    struct queue_head_t head;
    sendbyte_func func;
};

extern const char* keyboard_dev_name;
extern struct queue_def_t keyboard_qu;

extern void keyboard_init();
extern void keyboard_ih_low(uint32_t number, struct ih_low_data_t* data);
extern void keyboard_ih_high(struct message_t* msg);

extern void keyboard_ioctl(uint32_t command, uint32_t subcm);

extern uint32_t keyboard_write(void *buf, uint32_t *offset, uint32_t size);
// extern void driver_write_ch(char ch);

extern uint32_t keyboard_read(void *buf, uint32_t *offset, uint32_t size);
//extern char driver_read_ch(FILE* io_buf);

#endif
