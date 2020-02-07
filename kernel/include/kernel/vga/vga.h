#include <types.h>

#ifndef KERNEL_VGA_VGA_H_
#define KERNEL_VGA_VGA_H_

#define V_BLACK 0x00
#define V_BLUE 0x01
#define V_GREEN 0x02
#define V_CYAN 0x03
#define V_RED 0x04
#define V_MAGENTA 0x05
#define V_BROWN 0x06
#define V_LGRAY 0x07

#define V_DGRAY 0x08
#define V_LBLUE 0x09
#define V_LGREEN 0x0A
#define V_LCYAN 0x0B
#define V_LRED 0x0C
#define V_LMAGENTA 0x0D
#define V_YELLOW 0x0E
#define V_WHITE 0x0F

#define G_BLACK "\e\x00"
#define G_BLUE "\e\x01"
#define G_GREEN "\e\x02"
#define G_CYAN "\e\x03"
#define G_RED "\e\x04"
#define G_MAGENTA "\e\x05"
#define G_BROWN "\e\x06"
#define G_LGRAY "\e\x07"

#define G_DGRAY "\e\x08"
#define G_LBLUE "\e\x09"
#define G_LGREEN "\e\x0A"
#define G_LCYAN "\e\x0B"
#define G_LRED "\e\x0C"
#define G_LMAGENTA "\e\x0D"
#define G_YELLOW "\e\x0E"
#define G_WHITE "\e\x0F"

// i think there this is useless
#define TERM_COUNT 0
// x
#define VGA_COLS 80
// y
#define VGA_ROWS 25

#define VGA_SIZE VGA_COLS *VGA_ROWS
struct VGA_state_t {
    int term_col;
    int term_row;
    uint8_t screen;
    uint16_t *term_buffer;
    uint8_t term_color;
    uint8_t allow_legacy_vga_functions; // this is a little shitcode solution, but why not.
    // this VGA funcs we need only in case of panic or during startup.
};

extern volatile uint16_t *vga_buffer;
extern struct VGA_state_t vga_state;

extern uint16_t term_buffers[VGA_SIZE * (TERM_COUNT + 1)];

extern void term_init();
extern void term_clear();

extern void term_setc(uint16_t x, uint16_t y, uint8_t color, char c);  // set char directly into buffer, x,y, color, char;
extern void term_putc(char c, uint8_t flush);                          // set char directly into buffer, but with pos and suitable flush
extern void term_print(const char *str);                               // write string, autoflush

extern void term_change(uint8_t term_id);
extern void term_flush();  // copy buffer to real video buff
#endif
