#ifndef TYPES_H_
#define TYPES_H_

#define NULL  0
#define FALSE 0
#define TRUE  1

/* Convenience types.  */
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;

/* Fixed-size types, underlying types depend on word size and compiler.  */
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
#if __WORDSIZE == 64
typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;
#endif

typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;

#if __WORDSIZE == 64
typedef __uint64_t uint64_t;
#endif

#if __WORDSIZE == 64
typedef __uint64_t size_t;
#else
typedef __uint32_t size_t;
#endif

// typedef uint8_t bool;

typedef uint8_t *va_list;

#define va_start(ap, A) \
    (void)((ap) = (uint8_t *)&(A) + sizeof(A))

#define va_end(ap) \
    (void)((ap) = 0)

#define va_arg(ap, T) \
    (*(T *)(((ap) += sizeof(T)) - sizeof(T)))

#endif
