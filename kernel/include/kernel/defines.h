#include <types.h>

#ifndef KERNEL_DEFINES_H_
#define KERNEL_DEFINES_H_

#define VIRT_BASE ((size_t)0xC0000000)  // 3GB
#define PAGE_SIZE (0x1000)              // 4KB
#define TABLE_SIZE (PAGE_SIZE * 1024)   // 4 MB
#define PHYS(x) ((x)-VIRT_BASE)
#define VIRT(x) ((x) + VIRT_BASE)

#define TASK_QUOTA 3
#define IPC_MSG_DATA_BUFF_SIZE 256
#define TASK_MSG_BUFF_SIZE 8

#ifdef _DEBUG
#define DEBUG_ASM asm("xchg %bx, %bx");
#else
#define DEBUG_ASM
#endif

#ifndef KERNEL_VERSION
#define KERNEL_VERSION "vN.N.N"
#endif

#endif
