#include <kernel/DT/int.h>
#include <kernel/dev/tty.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
#include <lib/string.h>

const char *tty_dev_name = "TTY0";

char tty_output_buff[VGA_SIZE];
char tty_input_buff[VGA_COLS];

char *tty_output_ptr = tty_output_buff;
char *tty_input_ptr = tty_input_buff;

uint8_t read_line_mode = 0;
uint8_t is_echo = 1;

void tty_init() {
    struct clist_head_t *entry;
    struct dev_t *dev;
    struct ih_low_t *ih_low;

    memset(tty_output_buff, 0, VGA_SIZE);
    memset(tty_input_buff, 0, VGA_COLS);

    dev = dev_create();

    memcpy(dev->name, tty_dev_name, strlen(tty_dev_name) + 1);  // TODO: strcpy

    dev->base_r = tty_input_buff;
    dev->base_w = tty_output_buff;

    //dev->read_fn = tty_read; // TODO: implement
    //dev->write_fn = tty_write;
    //dev->cmd_fn = tty_cmd;

    dev->ih_list.head = NULL;
    dev->ih_list.slot_size = sizeof(struct ih_low_t);
    entry = clist_insert_after(&dev->ih_list, dev->ih_list.tail);

    ih_low = (struct ih_low_t *)entry->data;
    ih_low->number = INT_KEYBOARD;
    ih_low->handler = tty_keyboard_ih_low;
}

void tty_keyboard_ih_low(uint32_t number, struct ih_low_data_t* data) {
    /* write character to input buffer */
    char* keycode = data->data;
    int index = *keycode;
    assert(index < 128);
    char ch = 0; // TODO: keyboard_map;

    if(ch != '\b' || !read_line_mode) {
        *tty_input_ptr++ = ch;
    }

    if(is_echo && ch != '\n' && ch != '\b') {
        *tty_output_ptr++ = ch;
    }

    if(read_line_mode && ch == '\b') {
        if(tty_input_ptr > tty_input_buff) {
            *--tty_input_ptr = '\0';
            *--tty_output_ptr = ' ';
        }
    }
    
    struct message_t msg;
    msg.type = IPC_MSG_TYPE_DQ_SCHED;
    msg.len = 4;
    *((size_t *)msg.data) = (size_t)tty_keyboard_ih_high;
    ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg);
}


void tty_keyboard_ih_high(struct message_t *msg) {
    // video_flush(tty_output_buff);
}

uint32_t tty_read(FILE* io_buf, void *buffer, uint32_t size) {
    char *ptr = buffer;

    return (size_t)ptr - (size_t)buffer;
}

char tty_read_ch(FILE* io_buf) {
    io_buf->eof = 0;
    io_buf->eol = 0; // idk

    

    return '\0';
}