#include <kernel/defines.h>
#include <types.h>

#ifndef LIB_STDIO_H_
#define LIB_STDIO_H_

#ifdef KERNEL
#define assert(expr) kassert(__FILE__, __LINE__, expr)
#define printf(format, ...) kprintf(format, ##__VA_ARGS__)
#define vprintf(format, arg_list) kvprintf(format, arg_list)
#define clear() kclear();
#define flush() kflush();

extern void kclear();
extern void kvprintf(char *format, va_list arg_list);
extern void kprintf(char *format, ...);
extern void kmode(uint8_t mode);
extern void klog();
extern void kflush();

extern void kassert(const char *file, uint32_t line, uint8_t expr);
extern void kpanic(char *message, ...);

#else
extern void vprintf(char *format, va_list arg_list);
extern void printf(char *format, ...);
#endif

#endif
