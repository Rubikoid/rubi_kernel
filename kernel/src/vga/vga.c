#include <stddef.h>
#include <stdint.h>

#include <kernel/vga/vga.h>

volatile uint16_t* vga_buffer = (uint16_t*)(0xC0000000 + 0xB8000);
// By default, the VGA textmode buffer has a size of 80x25 characters
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

struct VGA_State vga_state;

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
}

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

    if (vga_state.term_row >= VGA_ROWS) { // TODO: make normal scrolling for vga
        vga_state.term_col = 0;
        vga_state.term_row = 0;
    }
}

void term_print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++)
        term_putc(str[i]);
}

void term_print_int(uint32_t x, uint32_t base) {
    char rt[36]; // more than 32 bits can't be. so.

    uint8_t i = 0;
    uint32_t k = 0;

    // make reversed string from number
    while (x != 0) {
        k = x % base;
        x = x / base;
        rt[i] = k + (k < 10 ? '0' : 'A' - 10);
        i++;
    }
    rt[i] = '\0';

    // reverse string
    i -= 1;
    k = 0;  // FIXME: FIX THAT SHITCODE!11
    for (int j = 0; j < i; j++, i--) {
        k = rt[j];
        rt[j] = rt[i];
        rt[i] = k;
    }
    term_print(rt);
}
