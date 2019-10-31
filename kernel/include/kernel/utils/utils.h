#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL_UTILS_UTILS_H_
#define KERNEL_UTILS_UTILS_H_

uint32_t strlen(char *str);
void itoa(char *ret, uint32_t x, uint32_t base);
void reverse_string(char *str, uint32_t size);

#endif
