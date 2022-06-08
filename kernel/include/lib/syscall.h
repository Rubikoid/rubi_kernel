#include <types.h>

#ifndef LIB_SYSCALL_H_
#define LIB_SYSCALL_H_

#include <lib/asm_lib.h>

#ifdef KERNEL
#include <kernel/memory/heap.h>
#include <kernel/scheduler/task.h>
#endif

#define NORMAL_SYSCALL_COUNT 11

#define SYSCALL_EXIT         0x1
#define SYSCALL_KSEND        0x2
#define SYSCALL_KRECV        0x3
#define SYSCALL_GETTID       0x4
#define SYSCALL_OPEN         0x5
#define SYSCALL_CLOSE        0x6
#define SYSCALL_READ         0x7
#define SYSCALL_WRITE        0x8
#define SYSCALL_IOCTL        0x9
#define SYSCALL_MALLOC       0xA
#define SYSCALL_FREE         0xB
#define SYSCALL_RESOLVE_PATH 0xC
#define SYSCALL_READDIR      0xD
//#define SYSCALL_FREE       0x0

#define SYSCALL_TEST 0x1337

#define syscall_exit() fsyscall(SYSCALL_EXIT, 0, 0, 0);

#define syscall_krecv(ptr) fsyscall(SYSCALL_KRECV, (uint32_t)ptr, 0, 0);
#define syscall_gettid()   fsyscall(SYSCALL_GETTID, 0, 0, 0);

// IO syscalls
#define syscall_open(path, mode)       fsyscall(SYSCALL_OPEN, (uint32_t)path, mode, 0);
#define syscall_close(fd)              fsyscall(SYSCALL_CLOSE, (uint32_t)fd, 0, 0);
#define syscall_read(fd, buf, size)    fsyscall(SYSCALL_READ, (uint32_t)fd, (uint32_t)buf, size);
#define syscall_write(fd, buf, size)   fsyscall(SYSCALL_WRITE, (uint32_t)fd, (uint32_t)buf, size);
#define syscall_ioctl(fd, cmd, subcmd) fsyscall(SYSCALL_IOCTL, (uint32_t)fd, cmd, subcmd);

#define syscall_resolve_path(path)            (struct fs_node_t *)fsyscall(SYSCALL_RESOLVE_PATH, (uint32_t)path, 0, 0);
#define syscall_readdir(fs_node, num, dirent) fsyscall(SYSCALL_READDIR, (uint32_t)fs_node, num, (uint32_t)dirent);

#define syscall_test(a, b) fsyscall(SYSCALL_TEST, (uint32_t)a, (uint32_t)b, 0);

#ifndef KERNEL

#define syscall_ksend(tid, ptr) fsyscall(SYSCALL_KSEND, (uint32_t)tid, (uint32_t)ptr, 0);
#define syscall_malloc(size)    (void *)fsyscall(SYSCALL_MALLOC, size, 0, 0);
#define syscall_free(addr)      fsyscall(SYSCALL_FREE, (uint32_t)addr, 0, 0);

#else

// if we in a kernel space, we can directly use some functions, instead of doing syscall
#define syscall_ksend(tid, ptr) ksend(tid, ptr);
#define syscall_malloc(size)    (void *)kmalloc(size);
#define syscall_free(addr)      kfree(addr);

#endif

#endif
