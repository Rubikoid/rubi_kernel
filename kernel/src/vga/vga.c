#include <stddef.h>
#include <stdint.h>

#include <kernel/utils/memory.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>

volatile uint16_t *vga_buffer = (uint16_t *)(0xC0000000 + 0xB8000);
// By default, the VGA textmode buffer has a size of 80x25 characters
const int VGA_COLS = 80;  // x
const int VGA_ROWS = 25;  // y

struct VGA_State vga_state = {0};

void term_init() {
    vga_state.term_col = 0;
    vga_state.term_row = 0;
    vga_state.term_color = 0x0F;
    term_clear();
}

void term_clear() {
    for (int col = 0; col < VGA_COLS; col++) {
        for (int row = 0; row < VGA_ROWS; row++) {
            // The VGA textmode buffer has size (VGA_COLS * VGA_ROWS).
            const size_t index = (VGA_COLS * row) + col;
            // Entries in the VGA buffer take the binary form BBBBFFFFCCCCCCCC, where:
            // - B is the background color
            // - F is the foreground color
            // - C is the ASCII character
            vga_buffer[index] = ((uint16_t)vga_state.term_color << 8) | ' ';
        }
    }
    vga_state.term_col = 0;
    vga_state.term_row = 0;
}

// x - cols, max 80; y - rows, max 25
void term_setc(uint16_t x, uint16_t y, uint8_t color, char c) {
    // check for newline, i because why not.
    if (c != '\n') {
        const size_t index = (VGA_COLS * y) + x;
        vga_buffer[index] = (color << 8) | c;
    }
}

void term_putc(char c) {
    switch (c) {
        case '\n': {
            vga_state.term_col = 0;
            vga_state.term_row++;
            break;
        }

        default: {
            term_setc(vga_state.term_col, vga_state.term_row, vga_state.term_color, c);
            vga_state.term_col++;
            break;
        }
    }

    if (vga_state.term_col >= VGA_COLS) {
        vga_state.term_col = 0;
        vga_state.term_row++;
    }

    if (vga_state.term_row >= VGA_ROWS) {  // TODO: make normal scrolling for vga
        vga_state.term_col = 0;
        vga_state.term_row = 0;
    }
}

void term_print(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++)
        term_putc(str[i]);
}

void vsprintf(char *ret, char *format, va_list arg_list) {
    uint32_t i = 0, j = 0;

    char rt[32];
    uint32_t num = 0;

    while (format[i] != 0) {
        switch (format[i]) {
            case '%':
                switch (format[i + 1]) {  // FIXME: i think, that we can't have more than uint32_t
                    case 'x':
                        num = va_arg(arg_list, uint32_t);
                        itoa(rt, num, 16);
                        j += memcpy(rt, (ret + j), strlen(rt));
                        i += 2;
                        break;
                    case 'u':
                        num = va_arg(arg_list, uint32_t);
                        itoa(rt, num, 10);
                        j += memcpy(rt, (ret + j), strlen(rt));
                        i += 2;
                        break;
                    case 's':
                        break;
                    default:
                        break;
                }
                break;
            default:
                ret[j] = format[i];
                i++;
                j++;
                break;
        }
    }
    ret[j] = 0;
}

void sprintf(char *ret, char *format, ...) {
    va_list va;
    va_start(va, format);
    vsprintf(ret, format, va);
}

void vprintf(char *format, va_list arg_list) {
    char ret[256];
    for (int i = 0; i < 256; i++) ret[i] = 'X';  // mem leak checker
    vsprintf(ret, format, arg_list);
    term_print(ret);
}

void printf(char *format, ...) {
    va_list va;
    va_start(va, format);
    vprintf(format, va);
}
