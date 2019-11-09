#include <types.h>
#include <kernel/defines.h>

#ifndef KERNEL_VGA_VGA_H_
#define KERNEL_VGA_VGA_H_

struct VGA_state_t
{
    int term_col;
    int term_row;
    uint8_t term_color;
};


extern volatile uint16_t *vga_buffer;
extern struct VGA_state_t vga_state;

extern void term_init();
extern void term_clear();
extern void term_setc(uint16_t x, uint16_t y, uint8_t color, char c);
extern void term_putc(char c);
extern void term_print(const char *str);
#endif
