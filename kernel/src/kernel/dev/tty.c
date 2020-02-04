#include <kernel/dev/tty.h>
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
    struct dev_t dev;
    struct ih_low_t *ih_low;

    memset(tty_output_buff, 0, VGA_SIZE);
    memset(tty_input_buff, 0, VGA_COLS);
}
