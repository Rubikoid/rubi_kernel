#include <stddef.h>
#include <stdint.h>

#include <kernel/defines.h>

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
void term_setc(uint16_t x, uint16_t y, uint8_t color, char c);
void term_putc(char c);
void term_print(const char* str);

void vsprintf(char *ret, char *format, va_list arg_list);
void sprintf(char *ret, char *format, ...);

void vprintf(char *format, va_list arg_list);
void printf(char *format, ...);
// TODO: implement printf

#endif
