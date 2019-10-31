#include <stddef.h>
#include <stdint.h>

#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>

uint32_t strlen(char *str) {
    uint32_t ret = 0;
    while (str[ret] != 0)
        ret += 1;
    return ret;
}

void itoa(char *ret, uint32_t x, uint32_t base) {
    uint32_t i = 0;
    uint32_t k = 0;

    if (x == 0) {  // patch from kernel panic if we try to print 0;
        ret[i] = '0';
        i++;
    }

    // make reversed string from number
    while (x != 0) {
        k = x % base;
        x = x / base;
        ret[i] = k + (k < 10 ? '0' : 'A' - 10);
        i++;
    }
    ret[i] = '\0';
    reverse_string(ret, i - 1);
}

void reverse_string(char *str, uint32_t size) {
    uint8_t k = 0;
    uint32_t j = 0;
    for (; j < size; j++, size--) {
        k = str[j];
        str[j] = str[size];
        str[size] = k;
    }
}
