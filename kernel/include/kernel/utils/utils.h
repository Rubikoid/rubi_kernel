#include <types.h>

#ifndef KERNEL_UTILS_UTILS_H_
#define KERNEL_UTILS_UTILS_H_

extern uint32_t strlen(char *str);
extern void itoa(char *ret, uint32_t x, uint32_t base);
extern void reverse_string(char *str, uint32_t size);

#endif
