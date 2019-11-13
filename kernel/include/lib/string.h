#include <types.h>

#ifndef LIB_STRING_H_
#define LIB_STRING_H_

/* 
 * POSIX string api
 */

extern int strlen(const char *s);
extern char *strcpy(char *dst, const char *src);
extern char *strncpy(char *dst, const char *src, uint32_t n);
extern void *memcpy(void *dst, const void *src, uint32_t bytes);
extern void *memset(void *dst, uint8_t value, uint32_t bytes);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, uint32_t n);
extern char *strcat(char *s1, const char *s2);
extern char *strext(char *buf, const char *str, char sym);
extern int strspn(char *str, const char *accept);
extern int strcspn(char *str, const char *rejected);
extern char *strchr(const char *str, char ch);
extern char *strtok_r(char *str, const char *delims, char **save_ptr);
extern char *memext(void *buff_dst, uint32_t n, const void *buff_src, char sym);
extern char *itoa(unsigned int value, char *str, unsigned int base);
extern unsigned int atou(char *str);
extern void strinv(char *str, uint32_t size);
extern unsigned int sprintf(char *dst, const char *format, ...);
// extern unsigned int snprintf(char *s1, uint32_t n, const char *s2, ...);
extern unsigned int vsprintf(char *dst, const char *format, va_list list);
// extern unsigned int vsnprintf(char *s1, unsigned int n, const char *s2, va_list list);

#endif
