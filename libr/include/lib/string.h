#include <types.h>

#ifndef LIB_STRING_H_
#define LIB_STRING_H_

/*
 * POSIX string api
 */

extern int strlen(const char *s);                                                 // ok
extern char *strcpy(char *dst, const char *src);                                  // ok
extern char *strncpy(char *dst, const char *src, uint32_t n);                     // ok
extern void *memcpy(void *dst, const void *src, uint32_t bytes);                  // ok
extern void *memset(void *dst, uint8_t value, uint32_t bytes);                    // ok
extern int memcmp(const void *s1, const void *s2, size_t n);                      // not ok
extern int strcmp(const char *s1, const char *s2);                                // ok
extern int strncmp(const char *s1, const char *s2, uint32_t n);                   // not ok
extern char *strcat(char *s1, const char *s2);                                    // not ok
extern char *strext(char *buf, const char *str, char sym);                        // not ok
extern int strspn(char *str, const char *accept);                                 // not ok
extern int strcspn(char *str, const char *rejected);                              // not ok
extern char *strchr(const char *str, char ch);                                    // not ok
extern char *strtok_r(char *str, const char *delims, char **save_ptr);            // not ok
extern char *memext(void *buff_dst, uint32_t n, const void *buff_src, char sym);  // not ok
extern char *itoa(unsigned int value, char *str, unsigned int base);              // ok
extern unsigned int atou(char *str);                                              // not ok
extern void strinv(char *str, uint32_t size);                                     // ok

extern unsigned int sprintf(char *dst, const char *format, ...);
// extern unsigned int snprintf(char *s1, uint32_t n, const char *s2, ...);
extern unsigned int vsprintf(char *dst, const char *format, va_list list);
// extern unsigned int vsnprintf(char *s1, unsigned int n, const char *s2, va_list list);

extern unsigned int sscanf(char *src, const char *format, ...);
extern unsigned int vsscanf(char *src, const char *format, va_list list);

#endif
