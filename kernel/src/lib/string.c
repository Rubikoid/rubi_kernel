#include <lib/string.h>

int strlen(const char *s) {
    uint32_t ret = 0;
    while (s[ret] != 0)
        ret += 1;
    return ret;
}

void *memcpy(void *buf1, const void *buf2, uint32_t bytes) {
    int i = 0;
    for (; i < bytes; i++) {
        ((uint8_t *)buf1)[i] = ((uint8_t *)buf2)[i];
    }
    return buf1;
}

char *itoa(unsigned int value, char *str, unsigned int base) {
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

unsigned int sprintf(char *s1, const char *s2, ...) {
    va_list va;
    va_start(va, s2);
    vsprintf(s1, s2, va);
    va_end(va);
}

unsigned int vsprintf(char *s1, const char *s2, va_list list) {
    uint32_t i = 0, j = 0;

    char rt[32];
    uint32_t num = 0;

    while (s2[i] != 0) {
        switch (s2[i]) {
            case '%':
                switch (s2[i + 1]) {  // FIXME: i think, that we can't have more than uint32_t
                    case 'x':
                        num = va_arg(list, uint32_t);
                        itoa(rt, num, 16);
                        memcpy((uint8_t *)rt, (uint8_t *)(s1 + j), strlen(rt));
                        j += strlen(rt);
                        i += 2;
                        break;
                    case 'u':
                        num = va_arg(list, uint32_t);
                        itoa(rt, num, 10);
                        memcpy((uint8_t *)rt, (uint8_t *)(s1 + j), strlen(rt));
                        j += strlen(rt);
                        i += 2;
                        break;
                    case 's':
                        break;
                    default:
                        break;
                }
                break;
            default:
                s1[j] = s2[i];
                i++;
                j++;
                break;
        }
    }
    s1[j] = 0;
}
