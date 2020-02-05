#include <kernel/defines.h>
#include <types.h>

#ifndef LIB_STDIO_H_
#define LIB_STDIO_H_

struct io_buf_t {
    uint32_t fd;    // file descriptor
    uint8_t *base;  // buffer start
    uint8_t *ptr;   // pos in buff
    uint8_t eof;    // if eof?
    void *file;     // file defenition
};

typedef struct io_buf_t FILE;

#ifdef KERNEL  // we are in kernel space
#define assert(expr) kassert(__FILE__, __func__, __LINE__, expr)
#define printf(format, ...) kprintf(format, ##__VA_ARGS__)
#define vprintf(format, arg_list) kvprintf(format, arg_list)

extern void kvprintf(char *format, va_list arg_list);  // kernel printf, works with magic vga.h and vsprintf from stdlib
extern void kprintf(char *format, ...);                // kernel printf, works with magic vga.h

extern void kassert(const char *file, const char *func, uint32_t line, uint8_t expr);
extern void kpanic(char *message, ...);  // panic function, resets vga state and write some error maybe with kprintf, so kpvrintf must not contain bugs!!

#else  // we are in user space
extern void vprintf(char *format, va_list arg_list);
extern void printf(char *format, ...);
#endif

#endif
