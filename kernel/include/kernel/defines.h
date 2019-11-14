#include <types.h>

#ifndef KERNEL_DEFINES_H_
#define KERNEL_DEFINES_H_

#define VIRT_BASE (0xC0000000)         // 3GB
#define PAGE_SIZE (0x1000)             // 4KB
#define TABLE_SIZE (PAGE_SIZE * 1024)  // 4 MB
#define KERNEL_TABLE_COUNT (4)
#define PHYS(x) ((x)-VIRT_BASE)
#define VIRT(x) ((x) + VIRT_BASE)

#ifdef _DEBUG
#define DEBUG_ASM asm("xchg %bx, %bx");
#else
#define DEBUG_ASM
#endif

#ifndef KERNEL_VERSION
#define KERNEL_VERSION "vN.N.N"
#endif

#endif
