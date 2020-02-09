#include <kernel/DT/int.h>
#include <kernel/DT/keyboard.h>
#include <kernel/asm_lib.h>
#include <kernel/dev/kb_driver.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
#include <kernel/memory/heap.h>
#include <lib/string.h>

#define __MODULE_NAME__ "KBD"

const char *keyboard_dev_name = "KB_LOW";

uint8_t kb_status = KB_STATUS_IDLE;
uint8_t kb_last_byte = 0;
uint8_t kb_status_before_resend = 0;
sendbyte_func last_func = 0;

struct queue_def_t keyboard_qu = {
    .clist_head = {
        .slots = 0,
        .slot_size = sizeof(struct kb_queue_t),
        .head = 0,
    },
};

uint8_t kb_echo_func(struct keyboard_status_t *status);

void keyboard_init() {
    struct clist_head_t *entry;
    struct dev_t *dev;
    struct ih_low_t *ih_low;

    dev = dev_create();

    strncpy(dev->name, keyboard_dev_name, sizeof(dev->name) - 1);

    dev->base_r = 0;
    dev->base_w = 1;

    dev->read_fn = keyboard_read;
    dev->write_fn = keyboard_write;
    dev->cmd_fn = keyboard_ioctl;

    dev->ih_list.head = NULL;
    dev->ih_list.slot_size = sizeof(struct ih_low_t);
    entry = clist_insert_after(&dev->ih_list, dev->ih_list.tail);

    ih_low = (struct ih_low_t *)entry->data;
    ih_low->number = INT_KEYBOARD;
    ih_low->subnumber = INT_KEYBOARD_NEW_DATA;
    ih_low->handler = keyboard_ih_low;
}

void keyboard_ih_low(uint32_t number, struct ih_low_data_t *data) {
    struct keyboard_status_t *status = data->data;
    uint8_t send_byte = 0;
    switch (status->keycode) {
        case KB_DATA_ECHO: {
            send_byte = last_func(status);
            break;
        }
        case KB_DATA_ACK: {
            if (kb_status >= KB_STATUS_RESEND0 || kb_status <= KB_STATUS_RESEND2)
                kb_status = kb_status_before_resend;
            send_byte = last_func(status);
            break;
        }
        case KB_DATA_RESEND: {
            if (kb_status >= KB_STATUS_RESEND0 || kb_status <= KB_STATUS_RESEND2) {
                kb_status += 1;  // we go to next status;
                send_byte = kb_last_byte;
            } else if (kb_status == KB_STATUS_FAILED) {  // fail after 3 times
                send_byte = last_func(status);
            } else {  // if kb want to resend
                kb_status_before_resend = kb_status;
                kb_status = KB_STATUS_RESEND0;
                send_byte = kb_last_byte;
            }
            break;
        }
        default:
            break;
    }
    if (send_byte != 0) {
        if (!(status->status & 0x2)) {  // input buffer empty
            kb_last_byte = send_byte;
            outb(KB_PORT, send_byte);
        } else {
            klog("Sending error line busy\n");
        }
    }
    /*struct message_t msg;  // create update task
    msg.type = IPC_MSG_TYPE_DQ_SCHED;
    msg.len = 4;
    msg.data = kmalloc(4);
    *((size_t *)msg.data) = (size_t)keyboard_ih_high;
    ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg); */
}

void send_data() {
    struct keyboard_status_t status;
    status.status = inb(KB_CMD_PORT);
    status.keycode = 0;

    uint8_t send_byte = 0;
    send_byte = last_func(&status);

    if (send_byte != 0) {
        if (!(status.status & 0x2)) {  // input buffer empty
            kb_last_byte = send_byte;
            outb(KB_PORT, send_byte);
        } else {
            klog("Sending data error line busy\n");
        }
    }
}

uint8_t kb_echo_func(struct keyboard_status_t *status) {
    uint8_t ret = 0;
    switch (kb_status) {
        case KB_STATUS_IDLE: {
            kb_status = KB_STATUS_IN_WORK0;
            return KB_DATA_ECHO;
            break;
        }
        case KB_STATUS_IN_WORK0: {
            if (status->keycode == KB_DATA_ECHO)
                klog("Echo check ok\n");
            else
                klog("Echo check watafack %x\n", status->keycode);
            status = KB_STATUS_IDLE;
            break;
        }
        case KB_STATUS_FAILED: {
            klog("Echo check failed?\n");
            status = KB_STATUS_IDLE;
            break;
        }
        default:
            break;
    }
    return ret;
}

void keyboard_ih_high(struct message_t *msg) {
}

void keyboard_ioctl(struct io_buf_t *io_buf, uint32_t command) {
    switch (command) {
        case IOCTL_INIT: {
            break;
        }
        case IOCTL_FLUSH: {
            struct queue_head_t entry;
            queue_pop(&keyboard_qu, &entry);
            struct kb_queue_t *kbq = (struct kb_queue_t *)entry.data;
            last_func = kbq->func;
            send_data();
            break;
        }
        case KEYBOARD_IOCTL_ECHO: {
            struct queue_head_t *entry = queue_push(&keyboard_qu);
            struct kb_queue_t *kbq = (struct kb_queue_t *)entry->data;\
            kbq->func = kb_echo_func;
            break;
        }
        default: {
            break;
        }
    }
}

void keyboard_write(struct io_buf_t *io_buf, void *data, uint32_t size) {
}

/*
void keyboard_write_ch(struct io_buf_t *io_buf, char ch) {
}
*/

uint32_t keyboard_read(FILE *io_buf, void *buffer, uint32_t size) {
    return 0;
}

/*
char keyboard_read_ch(FILE *io_buf) {
    return '\0';
}
*/
