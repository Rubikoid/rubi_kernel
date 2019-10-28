#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL_VGA_H
#define KERNEL_VGA_H

volatile uint16_t* vga_buffer = (uint16_t*)(0xC0000000 + 0xB8000);
// By default, the VGA textmode buffer has a size of 80x25 characters
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

// We start displaying text in the top-left of the screen (column = 0, row = 0)
int term_col = 0;
int term_row = 0;
uint8_t term_color = 0x0F;  // Black background, White foreground

void term_init();
void term_clear();
void term_putc(char c);
void term_print(const char* str);

void term_print_hex(uint8_t x);

#endif
