#include <lib/stdio.h>

#include <kernel/asm_lib.h>
#include <kernel/vga/vga.h>
#include <lib/string.h>

void kvprintf(char *format, va_list arg_list) {
    char ret[256];
    vsprintf(ret, format, arg_list);
    term_print(ret);
}

void kprintf(char *format, ...) {
    va_list va;
    va_start(va, format);
    kvprintf(format, va);
    va_end(va);
}

void kassert(const char *file, const char *func, uint32_t line, uint8_t expr) {
    if (!expr) {
        kprintf("[" G_RED "ERR" G_WHITE "] Assert in %s at %u (%s)\n", file, line, func);
    }
}

/*
    Because we can get panic while panic, we should maximize safety of code in panics
*/
void kpanic(char *message, ...) {
    disable_int();
    va_list va;
    va_start(va, message);

    vga_state.term_buffer = (uint16_t *)(0xC0000000 + 0xB8000); 
    vga_state.term_col = 0;
    vga_state.term_row = 0;
    vga_state.term_color = V_BLACK << 4 | V_WHITE;

    kvprintf(message, va);
    va_end(va);
    halt();
}
