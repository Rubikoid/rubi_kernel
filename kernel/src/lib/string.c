#include <lib/string.h>
#include <lib/stdio.h>

int strlen(const char *s) {
    uint32_t ret = 0;
    while (s[ret] != 0)
        ret += 1;
    return ret;
}

void *memset(void *dst, uint8_t value, uint32_t bytes) {
    int i = 0;
    for (; i < bytes; i++)
        ((uint8_t *)dst)[i] = value;
    return dst;
}

void *memcpy(void *buf1, const void *buf2, uint32_t bytes) {
    int i = 0;
    for (; i < bytes; i++)
        ((uint8_t *)buf1)[i] = ((uint8_t *)buf2)[i];
    return buf1;
}

//TODO: very haccky realization, fix that shit
int strcmp(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    if (len1 != len2)
        return 1337;  // so this is the hack and bad realization... but for if(!strcmp) this will work
    for (int i = 0; i < len1; i++) {
        if (s1[i] != s2[i])
            return 1338;  // also crazy implement;
    }
    return 0;
}

char *strcpy(char *dst, const char *src) {
    int i = 0;
    int len = strlen(src);
    for (; i < len; i++) {
        dst[i] = src[i];
    }
    dst[i] = 0;
    return dst;
}

char *strncpy(char *dst, const char *src, uint32_t n) {
    int i = 0;
    int len = strlen(src);
    for (; i < len && i < n; i++) {
        dst[i] = src[i];
    }
    dst[i] = 0;
    return dst;
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
    return vsprintf(s1, s2, va);
}

unsigned int vsprintf(char *s1, const char *s2, va_list list) {
    uint32_t src_i = 0, dst_i = 0;

    char rt[32];
    uint32_t num = 0;
    void *ptr = 0;

    while (s2[src_i] != 0) {
        switch (s2[src_i]) {
            case '%': {
                switch (s2[src_i + 1]) {  // FIXME: i think, that we can't have more than uint32_t
                    case 'x': {
                        num = va_arg(list, uint32_t);
                        itoa(num, rt, 16);
                        memcpy((uint8_t *)(s1 + dst_i), (uint8_t *)rt, strlen(rt));
                        dst_i += strlen(rt);
                        src_i += 2;
                        break;
                    }
                    case 'i':
                    case 'd':  // fuck this modifier i don't care about signed data ;D
                    case 'u': {
                        num = va_arg(list, uint32_t);
                        itoa(num, rt, 10);
                        memcpy((uint8_t *)(s1 + dst_i), (uint8_t *)rt, strlen(rt));
                        dst_i += strlen(rt);
                        src_i += 2;
                        break;
                    }
                    case 's': {
                        ptr = va_arg(list, void *);
                        memcpy((uint8_t *)(s1 + dst_i), (uint8_t *)ptr, strlen(ptr));
                        dst_i += strlen(ptr);
                        src_i += 2;
                        break;
                    }
                    case 'c': {
                        num = va_arg(list, uint32_t);
                        s1[dst_i] = (uint8_t)num;
                        dst_i += 1;
                        src_i += 2;
                    }
                    default: {
                        src_i += 1;
                        break;
                    }
                }
                break;
            }
            default: {
                s1[dst_i] = s2[src_i];
                src_i++;
                dst_i++;
                break;
            }
        }
    }
    s1[dst_i] = 0;
    return dst_i;
}

unsigned int sscanf(char *src, const char *format, ...) {
    va_list va;
    va_start(va, format);
    return vsscanf(src, format, va);
}

unsigned int vsscanf(char *src, const char *format, va_list list) {
    uint32_t format_i = 0, dst_i = 0;
    uint32_t count = 0;

    //char rt[32];
    //uint32_t num = 0;
    void *ptr = 0;

    while (format[format_i] != '\0' && src[dst_i] != '\0') {
        // printf("%x:%x\n", dst_i, src[dst_i]);
        switch (format[format_i]) {
            case '%': {
                switch (format[format_i + 1]) {  // FIXME: i think, that we can't have more than uint32_t
                    /*case 'x': {
                        num = va_arg(list, uint32_t);
                        itoa(num, rt, 16);
                        memcpy((uint8_t *)(dst + dst_i), (uint8_t *)rt, strlen(rt));
                        dst_i += strlen(rt);
                        format_i += 2;
                        break;
                    }
                    case 'd':  // fuck this modifier i don't care about signed data ;D
                    case 'u': {
                        num = va_arg(list, uint32_t);
                        itoa(num, rt, 10);
                        memcpy((uint8_t *)(dst + dst_i), (uint8_t *)rt, strlen(rt));
                        dst_i += strlen(rt);
                        format_i += 2;
                        break;
                    }*/
                    case 's': {
                        ptr = va_arg(list, void *);
                        while (src[dst_i] != '\0' && src[dst_i] != ' ' && src[dst_i] != '\n' && src[dst_i] != '\t') {
                            *(uint8_t *)ptr = src[dst_i];
                            ptr++;
                            dst_i++;
                        }
                        if(src[dst_i] == ' ' || src[dst_i] == '\n' || src[dst_i] == '\t')
                            dst_i++;
                        *(uint8_t *)ptr = '\0';
                        // memcpy((uint8_t *)(dst + dst_i), (uint8_t *)ptr, strlen(ptr));
                        // dst_i += strlen(ptr);
                        count += 1;
                        format_i += 2;
                        break;
                    }
                    /*case 'c': {
                        // num = va_arg(list, uint32_t);
                        // dst[dst_i] = (uint8_t)num;
                        dst_i += 1;
                        format_i += 2;
                    }*/
                    default: {
                        format_i += 1;
                        break;
                    }
                }
                break;
            }
            default: {
                // dst[dst_i] = format[format_i];
                format_i++;
                // dst_i++;
                break;
            }
        }
    }
    // dst[dst_i] = 0;
    return count;
}
