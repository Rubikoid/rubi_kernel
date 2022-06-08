#include <kernel/DT/int.h>
#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vfs/file.h>
#include <kernel/vfs/node.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <lib/syscall.h>
#include <types.h>

#define __MODULE_NAME__ "SCALL"

/*
static const char *syscall_names[NORMAL_SYSCALL_COUNT + 1] = {
    "SYSCALL_NONE",
    "SYSCALL_EXIT",
    "SYSCALL_KSEND",
    "SYSCALL_KRECV",
    "SYSCALL_GETTID",
    "SYSCALL_OPEN",
    "SYSCALL_CLOSE",
    "SYSCALL_READ",
    "SYSCALL_WRITE",
    "SYSCALL_IOCTL",
    "SYSCALL_MALLOC",
    "SYSCALL_FREE",
};
*/

#define ARG0 in_ebx
#define ARG1 in_ecx
#define ARG2 in_edx
#define ARG3 in_esi
#define ARG4 in_edi

uint32_t cint_syscall(PUSHAD_C) {
    struct task_t *task = current_task;
    // eax - syscall number
    // ebx - 1 param
    // ecx - 2 param
    // edx - 3 param
    // esi - 4 param
    // edi - 5 param
    // something else in structures or on stack.

    // klog(MSG_SYSCALL, syscall_names[in_eax % (NORMAL_SYSCALL_COUNT + 1)], in_eax, task == NULL ? ("NON") : (task->name), task == NULL ? 9999 : task->tid);

    uint32_t ret = 0;
    disable_int();
    switch (in_eax) {
        case SYSCALL_EXIT: {  // exit from current thread
            printf(MSG_SYSCALL_EXIT, task == NULL ? 9999 : task->tid);
            current_task->status = TASK_KILLING;
            sched_yield();
            break;
        }
        case SYSCALL_KSEND: {  // send IPC
            ksend(ARG0, (struct message_t *)ARG1);
            break;
        }
        case SYSCALL_KRECV: {  // recv IPC
            krecive(task->tid, (struct message_t *)ARG0);
            break;
        }
        case SYSCALL_GETTID: {          // get current task id
            ret = (uint32_t)task->tid;  // tid - 16, return - 32. вдруг что сломается?
            break;
        }
        case SYSCALL_OPEN: {  // open file
            // klog("OPEN %s %x\n", in_ebx, in_ecx);
            ret = file_open((char *)ARG0, (uint16_t)ARG1);
            break;
        }
        case SYSCALL_CLOSE: {  // close file
            break;
        }
        case SYSCALL_READ: {  // read file
            ret = file_read((uint32_t)ARG0, (uint8_t *)ARG1, ARG2);
            break;
        }
        case SYSCALL_WRITE: {  // write file
            ret = file_write((uint32_t)ARG0, (uint8_t *)ARG1, ARG2);
            // file_write((FILE *)in_ebx, (char *)in_ecx, in_edx);
            break;
        }
        case SYSCALL_IOCTL: {  // ioctl file
            ret = file_ioctl((uint32_t)ARG0, (uint32_t)ARG1, (uint32_t)ARG2);
            // file_ioctl((FILE *)in_ebx, in_ecx);
            break;
        }
        case SYSCALL_RESOLVE_PATH: {
            ret = (uint32_t)resolve_path((char *)ARG0);
            break;
        }
        case SYSCALL_READDIR: {
            ret = ((struct fs_node_t *)(ARG0))->readdir((struct fs_node_t *)ARG0, ARG1, (struct dirent_t *)ARG2);
            break;
        }
        case SYSCALL_MALLOC: {
            ret = (uint32_t)kmalloc(ARG0);
            break;
        }
        case SYSCALL_FREE: {
            kfree((void *)ARG0);
            break;
        }
        case SYSCALL_TEST: {  // test syscalls works, just sum first and second params
            printf(MSG_SYSCALL_TEST);
            ret = ARG0 + in_ecx;
            break;
        }
        default:  // do nothing.
            break;
    }
    enable_int();
    return ret;
}
