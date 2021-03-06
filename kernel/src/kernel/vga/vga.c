#include <kernel/asm_lib.h>
#include <kernel/serial/serial.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>
#include <lib/string.h>
#include <types.h>

volatile uint16_t *vga_buffer = (uint16_t *)(0xC0000000 + 0xB8000);

// By default, the VGA textmode buffer has a size of 80x25 characters
uint16_t term_buffers[VGA_SIZE * (TERM_COUNT + 1)] = {0};

struct VGA_state_t vga_state = {0};

void term_init() {
    vga_state.term_col = 0;
    vga_state.term_row = 0;
    vga_state.term_color = V_BLACK << 4 | V_WHITE;
    vga_state.screen = 0;
    vga_state.term_buffer = (uint16_t *)term_buffers + (VGA_SIZE * vga_state.screen);
    vga_state.allow_legacy_vga_functions = 1;
    //disable cursor
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    term_clear();
}

void term_clear() {
    //if(!vga_state.allow_legacy_vga_functions)
    //    return;
    for (int col = 0; col < VGA_COLS; col++) {
        for (int row = 0; row < VGA_ROWS; row++) {
            // The VGA textmode buffer has size (VGA_COLS * VGA_ROWS).
            const size_t index = (VGA_COLS * row) + col;
            // Entries in the VGA buffer take the binary form BBBBFFFFCCCCCCCC, where:
            // - B is the background color
            // - F is the foreground color
            // - C is the ASCII character
            vga_state.term_buffer[index] = ((uint16_t)vga_state.term_color << 8) | ' ';
        }
    }
    vga_state.term_col = 0;
    vga_state.term_row = 0;
    if (vga_state.allow_legacy_vga_functions)
        term_flush();
}

// x - cols, max 80; y - rows, max 25
void term_setc(uint16_t x, uint16_t y, uint8_t color, char c) {
    //if(!vga_state.allow_legacy_vga_functions)
    //    return;
    if (c != '\n') {  // check for newline
        const size_t index = (VGA_COLS * y) + x;
        vga_state.term_buffer[index] = (color << 8) | c;
    }
}

void term_putc(char c, uint8_t flush) {
    //if(!vga_state.allow_legacy_vga_functions)
    //    return;
    switch (c) {
        case '\0': {
            break;
        }
        case '\n': {
            write_com(0, c);
            write_com(0, '\r');
            vga_state.term_col = 0;
            vga_state.term_row++;
            break;
        }
        case '\b': {
            vga_state.term_col--;
            term_setc(vga_state.term_col, vga_state.term_row, vga_state.term_color, ' ');
            break;
        }
        default: {
            write_com(0, c);
            term_setc(vga_state.term_col, vga_state.term_row, vga_state.term_color, c);
            vga_state.term_col++;
            break;
        }
    }

    if (vga_state.term_col >= VGA_COLS) {
        vga_state.term_col = 0;
        vga_state.term_row++;
    }

    if (vga_state.term_col < 0) {
        vga_state.term_col = VGA_COLS - 1;
        vga_state.term_row--;
    }

    if (vga_state.term_row < 0)  // so we don't have buffer for that shit...
        vga_state.term_row = 0;

    if (vga_state.term_row >= VGA_ROWS) {
        // vga_state.term_col = 0;
        // vga_state.term_row = 0;
        for (int y = 1; y < VGA_ROWS; y++) {
            for (int x = 0; x < VGA_COLS; x++) {
                vga_state.term_buffer[VGA_COLS * (y - 1) + x] = vga_state.term_buffer[VGA_COLS * y + x];
            }
        }
        for (int x = 0; x < VGA_COLS; x++) {
            vga_state.term_buffer[(VGA_COLS * (VGA_ROWS - 1)) + x] = ((uint16_t)vga_state.term_color << 8) | ' ';
        }
        vga_state.term_row = VGA_ROWS - 1;
    //    if (flush || vga_state.allow_legacy_vga_functions)
    //        term_flush();
    }
    //if (flush)
    //    term_flush();
}

void term_print(const char *str) {
    // if(!vga_state.allow_legacy_vga_functions)
    //    return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '\e': {
                vga_state.term_color = str[++i];
                break;
            }
            default: {
                //write_com(0, str[i]);
                //if (str[i] == '\n')
                //    write_com(0, '\r');  // we ignore \r character, but seems like serial need to have it
                term_putc(str[i], 0);
                break;
            }
        }
    }
    term_flush();
}

void term_change(uint8_t term_id) {
    if (!vga_state.allow_legacy_vga_functions)
        return;
    vga_state.screen = 0;
    vga_state.term_buffer = (uint16_t *)term_buffers + (VGA_SIZE * vga_state.screen);
    term_flush();
}

void term_flush() {
    memcpy((void *)vga_buffer, vga_state.term_buffer, VGA_SIZE * 2);  // * 2 because uint16
}
