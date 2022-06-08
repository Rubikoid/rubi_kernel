#include <kernel/DT/int.h>
#include <kernel/DT/keyboard.h>
#include <kernel/dev/tty.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
#include <kernel/memory/heap.h>
#include <kernel/serial/serial.h>
#include <lib/string.h>

// const char *tty_dev_name = "TTY";

#define __MODULE_NAME__ "DRV_" tty_dev_name

// keyboard map for char set 1
unsigned char keyboard_map[KEYBOARD_MAP_SIZE] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
    '9', '0', '-', '=', '\b',                         /* Backspace */
    '\t',                                             /* Tab */
    'q', 'w', 'e', 'r',                               /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     /* Enter key */
    0,                                                /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`', 0,                                     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',               /* 49 */
    'm', ',', '.', '/', 0,                            /* Right shift */
    '*', 0,                                           /* Alt */
    ' ',                                              /* Space bar */
    0,                                                /* Caps lock */
    0,                                                /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0, 0,                        /* < ... F10 */
    0,                                                /* 69 - Num lock*/
    0,                                                /* Scroll Lock */
    0,                                                /* Home key */
    0,                                                /* Up Arrow */
    0,                                                /* Page Up */
    '-', 0,                                           /* Left Arrow */
    0, 0,                                             /* Right Arrow */
    '+', 0,                                           /* 79 - End key*/
    0,                                                /* Down Arrow */
    0,                                                /* Page Down */
    0,                                                /* Insert Key */
    0,                                                /* Delete Key */
    0, 0, 0, 0,                                       /* F11 Key */
    0,                                                /* F12 Key */
    0,                                                /* All other keys are undefined */
};

struct interrupt_message_t {
    void *callback;
    uint32_t data;
};

char tty_output_buff[1];
char tty_input_buff[VGA_COLS];

char *tty_output_ptr = tty_output_buff;
char *tty_input_ptr = tty_input_buff;

uint8_t read_line_mode = 1;
uint8_t is_echo = 0;

void tty_init() {
    struct clist_head_t *entry;
    struct dev_t *dev;
    struct ih_low_t *ih_low;

    memset(tty_output_buff, 0, 1);
    memset(tty_input_buff, 0, VGA_COLS);

    dev = dev_create();

    strncpy(dev->name, tty_dev_name, sizeof(dev->name) - 1);

    dev->base_r = tty_input_buff;
    dev->base_w = tty_output_buff;

    dev->read_fn = tty_read;
    dev->write_fn = tty_write;
    dev->cmd_fn = tty_ioctl;

    dev->ih_list.head = NULL;
    dev->ih_list.tail = NULL;
    dev->ih_list.slot_size = sizeof(struct ih_low_t);
    entry = clist_insert_after(&dev->ih_list, dev->ih_list.tail);

    ih_low = (struct ih_low_t *)entry->data;
    ih_low->number = INT_KEYBOARD;
    ih_low->subnumber = INT_KEYBOARD_NEW_KEY;
    ih_low->handler = tty_keyboard_ih_low;

    entry = clist_insert_after(&dev->ih_list, entry);
    ih_low = (struct ih_low_t *)entry->data;
    ih_low->number = INT_SERIAL;
    ih_low->subnumber = 1;
    ih_low->handler = tty_keyboard_ih_serial_low;

    klog("Handlers: %x, %x\n", tty_keyboard_ih_low, tty_keyboard_ih_serial_low);
}

void do_new_char_internal(char ch) {
    if (ch == 0)
        return;

    if (tty_input_ptr < tty_input_buff + VGA_COLS) {  // so we don't want to have memory leak
        // if (ch != '\b' || !read_line_mode) {          // not \b or not read line mode
        *tty_input_ptr++ = ch;
        //}
    }

    // if(tty_output_ptr < tty_output_buff + VGA_COLS) { // so we don't want to have memory leak
    if (is_echo && ch != '\0') {  // ch != '\n' && ch != '\b'
        //*tty_output_ptr++ = ch;
        if (tty_input_ptr > tty_input_buff) {  // and we not on the start of buffer
            term_putc(ch, FALSE);              // if not \n, not \b and echo enabled
        }
    }
    //}
    /*if (read_line_mode && ch == '\b') {        // if lines and \b
        if (tty_input_ptr > tty_input_buff) {  // and we not on the start of buffer
            *--tty_input_ptr = '\0';
            // term_putc(ch, FALSE); // we should handle \b
            //*--tty_output_ptr = ' ';
        }
    }*/

    // struct message_t msg;  // create update task
    // msg.type = IPC_MSG_TYPE_DQ_SCHED;
    // msg.len = sizeof(struct interrupt_message_t);
    // msg.data = kmalloc(msg.len);
    // ((struct interrupt_message_t *)msg.data)->callback = (void *)tty_keyboard_ih_high;
    // ((struct interrupt_message_t *)msg.data)->data = ch;
    // ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg);
}

void tty_keyboard_ih_low(uint32_t number, struct ih_low_data_t *data) {
    /* write character to input buffer */
    struct keyboard_status_t *keycode = data->data;
    int index = keycode->keycode;
    // assert(index < 128);
    if (index >= 128)  // this meant that key was released
        return;

    char ch = keyboard_map[index];  // TODO: keyboard_map;

    if (index == 59) {  // F1, just for debug.
        printf("\nCurrent input array: (%x, %x) %s\n", strlen(tty_input_buff), tty_input_ptr - tty_input_buff, tty_input_buff);
    }

    do_new_char_internal(ch);
}

void tty_keyboard_ih_serial_low(uint32_t number, struct ih_low_data_t *data) {
    // klog("serial task created");
    uint8_t index = *((uint8_t *)data->data);

    char ch = '\0';
    if (index == 0x7F) {
        ch = '\b';
    } else if (index == 0xD) {
        ch = '\n';
    } else if (index >= 0x4 && index <= 0xF) {
        ch = '\0';
    } else {
        ch = index;
    }
    /*
        ESC = 1B
        F1  = 4
        F2  = 5
        F3  = 6
        F4  = 7
        F5  = 8
        F6  = 9
        F7  = A
        F8  = B
        F9  = C
        F10 = D
        F11 = E
        F12 = F
        TAG = 9
    */

    do_new_char_internal(ch);

    // struct message_t msg;  // create update task
    // msg.type = IPC_MSG_TYPE_DQ_SCHED;
    // msg.len = sizeof(struct interrupt_message_t);
    // msg.data = kmalloc(msg.len);
    // ((struct interrupt_message_t *)msg.data)->callback = (void *)tty_keyboard_ih_high;
    // ((struct interrupt_message_t *)msg.data)->data = index;
    // ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg);
}

void tty_keyboard_ih_high(struct message_t *msg) {
    struct interrupt_message_t *interrupt_msg = msg->data;
    if (interrupt_msg->data != 0) {
        klog("serial_inp: %c (%x)\n", (uint8_t)interrupt_msg->data, (uint8_t)interrupt_msg->data);
    }
    term_flush();
}

void tty_ioctl(uint32_t command, uint32_t subcmd) {
    switch (command) {
        case IOCTL_INIT: {
            if (subcmd == TTY_IOCTL_WRITE && vga_state.allow_legacy_vga_functions) {
                term_print("Migrate to tty driver");
                // io_buf->ptr = (uint8_t *)tty_output_ptr;
                vga_state.allow_legacy_vga_functions = 0;
                {
                    uint32_t x = 0;
                    tty_write("... [OK]\n", &x, 9);
                }
                term_flush();
            }
            break;
        }
        case IOCTL_FLUSH: {
            if (subcmd == TTY_IOCTL_WRITE) {
                term_flush();
            }
            break;
        }
        case TTY_IOCTL_CLEAR: {
            if (subcmd == TTY_IOCTL_WRITE) {
                term_clear();
                term_flush();
            } else if (subcmd == TTY_IOCTL_READ) {
                // tty_input_ptr = tty_input_buff;
                // io_buf->ptr = (uint8_t *)tty_input_ptr;
                // io_buf->eof = TRUE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_LINE_ON: {
            if (subcmd == TTY_IOCTL_READ) {
                read_line_mode = TRUE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_ECHO_ON: {
            if (subcmd == TTY_IOCTL_READ) {
                is_echo = TRUE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_LINE_OFF: {
            if (subcmd == TTY_IOCTL_READ) {
                read_line_mode = FALSE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_ECHO_OFF: {
            if (subcmd == TTY_IOCTL_READ) {
                is_echo = FALSE;
            }
            break;
        }
        default: {
            break;
        }
    }
}

uint32_t tty_write(void *buf, uint32_t *offset, uint32_t size) {
    char *ptr = buf;
    char ch = -1;
    // io_buf->eof = 0;  // FIXME: i think this is a strange fix, but now it will work, so...

    for (int i = 0; i < size && ch != '\0' /*&& !io_buf->eof*/; i++) {
        ch = *ptr++;

        if (ch == '\e') {
            term_set_color(*ptr++);
            continue;
        }

        tty_write_ch(ch);
        // write_com(0, ch);
        // if (ch == '\n')
        //     write_com(0, '\r');  // we ignore \r character, but seems like serial need to have it
    }
    return (size_t)ptr - (size_t)buf;
}

void tty_write_ch(char ch) {
    /*if((size_t) tty_output_ptr < (size_t) tty_output_buff + VGA_SIZE - 1) {
        switch (ch) {
            case '\n': {
                int line_pos = ((size_t)tty_output_ptr - (size_t)tty_output_buff) % VGA_COLS;
                for(int i=0; i < VGA_COLS - line_pos; i++) {

                }
                //vga_state.term_col = 0;
                //vga_state.term_row++;
                break;
            }
            default: {
                //term_setc(vga_state.term_col, vga_state.term_row, vga_state.term_color, c);
                //vga_state.term_col++;
                break;
            }
        }*/
    //}
    if (ch != '\0')
        term_putc(ch, FALSE);
}

uint32_t tty_read(void *buf, uint32_t *offset, uint32_t size) {
    char *ptr = buf;
    char ch = '\0';
    do {
        ch = tty_read_ch(offset);
        if (read_line_mode && ch == '\b') {
            *ptr-- = '\0';
            size += 1;
        } else if (ch != '\0') {
            *ptr++ = ch;
            size -= 1;
            // printf("New symb: %x %x\n", ch, size);
        } else {
            sched_yield();
        }
    } while (size > 0 && (read_line_mode ? (ch != '\n') : (1)));  //  && !io_buf->eof
    // printf("Reason of death: %x %x\n", size, ch);
    return (size_t)ptr - (size_t)buf;
}

char tty_read_ch(uint32_t *offset) {
    uint8_t eof = (size_t)(tty_input_buff + *offset) >= (size_t)tty_input_ptr;  // pointer in file bigger than pointer in data;
    // io_buf->eol = 0;                                             // idk

    // there are some strange shit
    /*
    if (!io_buf->is_eof && read_line_mode) {
        // skip line
        *((char *)tty_input_buff_ptr) = '\0';
        if (strchr(io_buf->ptr, '\n') == null) {
            io_buf->is_eof = true;
            return '\0';
        }
    }
    */
    if (!eof) {
        char ch = *(tty_input_buff + *offset);
        if (read_line_mode && ch == '\n') {
            tty_input_ptr = tty_input_buff;  // reset to start
            *offset = 0;
            // return '\0';
        } else
            *offset = *offset + 1;
        return ch;
    } else {
        *offset = tty_input_ptr - tty_input_buff;
    }
    return '\0';
}
