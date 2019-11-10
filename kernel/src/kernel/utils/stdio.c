#include <lib/stdio.h>

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
