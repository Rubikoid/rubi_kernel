#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL_VGA_VGA_H_
#define KERNEL_VGA_VGA_H_

struct VGA_State
{
    int term_col;
    int term_row;
    uint8_t term_color;
};


extern volatile uint16_t* vga_buffer;
extern struct VGA_State vga_state;

void term_init();
void term_clear();
void term_putc(char c);
void term_print(const char* str);

void term_print_int(uint32_t x, uint32_t base);
void term_print_hex(uint8_t x);


#endif
