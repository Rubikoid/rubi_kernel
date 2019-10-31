#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL_DEFINES_H_
#define KERNEL_DEFINES_H_

#define VIRT_BASE (0xC0000000)
#define PAGE_SIZE (0x1000)
#define TABLE_SIZE (PAGE_SIZE * 1024)
#define KERNEL_TABLE_COUNT (4)

#ifdef _DEBUG
#define DEBUG_ASM asm("xchg %bx, %bx");
#else
#define DEBUG_ASM
#endif

#ifndef KERNEL_VERSION
#define KERNEL_VERSION "vN.N.N"
#endif

typedef uint8_t* va_list;

#define va_start(ap, A) \
    (void)((ap) = (uint8_t*)&(A) + sizeof(A))

#define va_end(ap) \
    (void)((ap) = 0)

#define va_arg(ap, T) \
    (*(T*)(((ap) += sizeof(T)) - sizeof(T)))

#endif
