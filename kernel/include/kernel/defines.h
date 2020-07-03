#include <types.h>

#ifndef KERNEL_DEFINES_H_
#define KERNEL_DEFINES_H_

#define VIRT_BASE       ((size_t)0xC0000000)           // 3GB
#define PAGE_SIZE       (0x1000)                       // 4KB
#define PAGES_PER_TABLE 1024                           // count of pages per table
#define TABLE_SIZE      (PAGE_SIZE * PAGES_PER_TABLE)  // 4 MB
#define PHYS(x)         ((x)-VIRT_BASE)                // convert addr to physical from kernel space. a little buggy macro.
#define VIRT(x)         ((x) + VIRT_BASE)              // convert addr to virtual from physical in kernel space. also buggy

#define TASK_QUOTA             3    // time quants for one task before switch
#define IPC_MSG_DATA_BUFF_SIZE 256  // buffer of IPC msgs
#define TASK_MSG_BUFF_SIZE     8

#ifdef _DEBUG
#define DEBUG_ASM asm("xchg %bx, %bx");  // insert magic debug instuction
#else
#define DEBUG_ASM ;  // insert EMPTY magic debug instuction, debug disabled
#endif

#ifndef KERNEL_VERSION
#define KERNEL_VERSION "vN.N.N"  // kernel version placeholder
#endif

#endif
