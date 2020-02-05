#include <types.h>

#ifndef KERNEL_DT_SYSCALL_H_
#define KERNEL_DT_SYSCALL_H_

#include <kernel/asm_lib.h>

#define SYSCALL_EXIT 0x1
#define SYSCALL_KSEND 0x2
#define SYSCALL_KRECV 0x3
#define SYSCALL_TEST 0x1337

#define syscall_exit() fsyscall(SYSCALL_EXIT, 0, 0, 0);
#define syscall_ksend(tid, ptr) fsyscall(SYSCALL_KSEND, (uint32_t)tid, (uint32_t)ptr, 0);
#define syscall_krecv(ptr) fsyscall(SYSCALL_KRECV, (uint32_t)ptr, 0, 0);
#define syscall_test(a, b) fsyscall(SYSCALL_TEST, (uint32_t)a, (uint32_t)b, 0);

#endif
