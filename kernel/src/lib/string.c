#include <lib/string.h>

int strlen(const char *s) {
    uint32_t ret = 0;
    while (s[ret] != 0)
        ret += 1;
    return ret;
}

void* memcpy(void *buf1, const void *buf2, uint32_t bytes) {
    int i = 0;
    for (; i < bytes; i++) {
        ((uint8_t *)buf1)[i] = ((uint8_t *)buf2)[i];
    }
    return buf1;
}

char* itoa(unsigned int value, char *str, unsigned int base) {
    uint32_t i = 0;
    uint32_t k = 0;

    if (value == 0) {  // patch from kernel panic if we try to print 0;
        str[i] = '0';
        i++;
    }

    // make reversed string from number
    while (value != 0) {
        k = value % base;
        value = value / base;
        str[i] = k + (k < 10 ? '0' : 'A' - 10);
        i++;
    }
    str[i] = '\0';
    strinv(str, i - 1);
    return str;
}

void strinv(char *str, uint32_t size) {
    uint8_t k = 0;
    uint32_t j = 0;
    for (; j < size; j++, size--) {
        k = str[j];
        str[j] = str[size];
        str[size] = k;
    }
}
