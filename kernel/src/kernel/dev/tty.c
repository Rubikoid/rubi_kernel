#include <kernel/DT/int.h>
#include <kernel/dev/tty.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
#include <kernel/memory/heap.h>
#include <lib/string.h>

const char *tty_dev_name = "TTY";

unsigned char keyboard_map[KEYBOARD_MAP_SIZE] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b', /* Backspace */
    '\t', /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0, /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`', 0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', /* 49 */
    'm', ',', '.', '/', 0, /* Right shift */
    '*', 0, /* Alt */
    ' ', /* Space bar */
    0, /* Caps lock */
    0, /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0, 0, /* < ... F10 */
    0, /* 69 - Num lock*/
    0, /* Scroll Lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-', 0, /* Left Arrow */
    0, 0, /* Right Arrow */
    '+', 0, /* 79 - End key*/
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0, 0, /* F11 Key */
    0, /* F12 Key */
    0, /* All other keys are undefined */
};

char tty_output_buff[1];
char tty_input_buff[VGA_COLS];

char *tty_output_ptr = tty_output_buff;
char *tty_input_ptr = tty_input_buff;

uint8_t read_line_mode = 1;
uint8_t is_echo = 1;


void tty_init() {
    struct clist_head_t *entry;
    struct dev_t *dev;
    struct ih_low_t *ih_low;

    memset(tty_output_buff, 0, 1);
    memset(tty_input_buff, 0, VGA_COLS);

    dev = dev_create();

    memcpy(dev->name, tty_dev_name, strlen(tty_dev_name) + 1);  // TODO: strcpy

    dev->base_r = tty_input_buff;
    dev->base_w = tty_output_buff;

    dev->read_fn = tty_read; // TODO: implement
    dev->write_fn = tty_write;
    dev->cmd_fn = tty_ioctl;

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
    char ch = keyboard_map[index]; // TODO: keyboard_map;

    if(tty_input_ptr < tty_input_buff + VGA_COLS) { // so we don't want to have memory leak
        if(ch != '\b' || !read_line_mode) { // not \b or not read line mode
            *tty_input_ptr++ = ch;
        }
    }

    //if(tty_output_ptr < tty_output_buff + VGA_COLS) { // so we don't want to have memory leak
        if(is_echo && ch != '\0') { // ch != '\n' && ch != '\b'
            //*tty_output_ptr++ = ch;
            term_putc(ch, FALSE); // if not \n, not \b and echo enabled
        }
    //}
    if(read_line_mode && ch == '\b') { // if lines and \b
        if(tty_input_ptr > tty_input_buff) { // and we not on the start of buffer
            *--tty_input_ptr = '\0';
            //term_putc(ch, FALSE); // we should handle \b
            //*--tty_output_ptr = ' ';
        }
    }
    
    struct message_t msg; // create update task
    msg.type = IPC_MSG_TYPE_DQ_SCHED;
    msg.len = 4;
    msg.data = kmalloc(4);
    *((size_t *)msg.data) = (size_t)tty_keyboard_ih_high;
    ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg);
}


void tty_keyboard_ih_high(struct message_t *msg) {
    term_flush();
}

void tty_ioctl(struct io_buf_t* io_buf, uint32_t command) {
    switch(command) {
        case IOCTL_INIT: {
            if((size_t)io_buf->base == (size_t)tty_output_buff) {
                term_print("Migrate to tty driver");
                io_buf->ptr = (uint8_t *)tty_output_ptr;
                vga_state.allow_legacy_vga_functions = 0;
                tty_write(io_buf, "... [OK]", 8);
                term_flush();
            }
            break;
        }
        case IOCTL_FLUSH: {
            if((size_t)io_buf->base == (size_t)tty_output_buff) {
                term_flush();
            }
        }
        case TTY_IOCTL_CLEAR: {
            if((size_t)io_buf->base == (size_t)tty_output_buff) {
                term_clear();
                term_flush();
            } else if((size_t)io_buf->base == (size_t)tty_input_buff) {
                tty_input_ptr = tty_input_buff;
                io_buf->ptr = (uint8_t *)tty_input_ptr;
                io_buf->eof = TRUE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_LINE_ON: {
            if((size_t)io_buf->base == (size_t)tty_input_buff) {
                read_line_mode = TRUE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_ECHO_ON: {
            if((size_t)io_buf->base == (size_t)tty_input_buff) {
                is_echo = TRUE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_LINE_OFF: {
            if((size_t)io_buf->base == (size_t)tty_input_buff) {
                read_line_mode = FALSE;
            }
            break;
        }
        case TTY_IOCTL_READ_MODE_ECHO_OFF: {
            if((size_t)io_buf->base == (size_t)tty_input_buff) {
                is_echo = FALSE;
            }
            break;
        }
        default: {
            break;
        }
    }
}

void tty_write(struct io_buf_t* io_buf, void* data, uint32_t size)
{
    char* ptr = data;

    for (int i = 0; i < size && !io_buf->eof; ++i) {
        tty_write_ch(io_buf, *ptr++);
    }
}

void tty_write_ch(struct io_buf_t* io_buf, char ch) {
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
    term_putc(ch, FALSE);
}


uint32_t tty_read(FILE* io_buf, void *buffer, uint32_t size) {
    char *ptr = buffer;

    do {
        char ch = tty_read_ch(io_buf);
        if (ch != '\0') {
            *ptr++ = ch;
            size -= 1;
            // printf("New symb: %x %x\n", ch, size);
        }
        else {
            sched_yield();
        }
    } while(size > 0 && (read_line_mode ? (!io_buf->eol) : (!io_buf->eof))); //  && !io_buf->eof
    printf("Reason of death: %x %x %x\n", size, io_buf->eof, io_buf->eol);
    return (size_t)ptr - (size_t)buffer;
}

char tty_read_ch(FILE* io_buf) {
    io_buf->eof = (size_t)io_buf->ptr >= (size_t)tty_input_ptr; // pointer in file bigger than pointer in data;
    io_buf->eol = 0; // idk

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
    if(!io_buf->eof) {
        char ch = *io_buf->ptr++;
        if(read_line_mode && ch == '\n') {
            io_buf->eof = 1;
            io_buf->eol = 1;
            tty_input_ptr = tty_input_buff; // reset to start
            io_buf->ptr = (uint8_t *)tty_input_ptr;
            return '\0';
        }
        return ch;
    }
    else {
        io_buf->ptr = (uint8_t *)tty_input_ptr;
    }
    return '\0';
}