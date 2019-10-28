#include <stddef.h>
#include <stdint.h>

#include <kernel/vga/vga.h>

void term_init() {
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
            vga_buffer[index] = ((uint16_t)term_color << 8) | ' ';
        }
    }
}

void term_putc(char c) {
    switch (c) {
        case '\n': {
            term_col = 0;
            term_row++;
            break;
        }

        default:
        {
            const size_t index = (VGA_COLS * term_row) + term_col;
            vga_buffer[index] = ((uint16_t)term_color << 8) | c;
            term_col++;
            break;
        }
    }

    if (term_col >= VGA_COLS) {
        term_col = 0;
        term_row++;
    }

    if (term_row >= VGA_ROWS) {
        term_col = 0;
        term_row = 0;
    }
}

void term_print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) 
        term_putc(str[i]);
}

void term_print_hex(uint8_t x) {
    
}
