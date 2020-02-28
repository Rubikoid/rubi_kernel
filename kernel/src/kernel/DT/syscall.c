#include <kernel/DT/int.h>
#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vfs/file.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <lib/syscall.h>
#include <types.h>

#define __MODULE_NAME__ "SCALL"

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
            ksend(in_ebx, (struct message_t *)in_ecx);
            break;
        }
        case SYSCALL_KRECV: {  // recv IPC
            krecive(task->tid, (struct message_t *)in_ebx);
            break;
        }
        case SYSCALL_GETTID: {          // get current task id
            ret = (uint32_t)task->tid;  // tid - 16, return - 32. вдруг что сломается?
            break;
        }
        case SYSCALL_OPEN: {  // open file
            // klog("OPEN %s %x\n", in_ebx, in_ecx);
            ret = file_open((char *)in_ebx, (uint16_t)in_ecx);
            break;
        }
        case SYSCALL_CLOSE: {  // close file
            break;
        }
        case SYSCALL_READ: {  // read file
            ret = file_read((uint32_t)in_ebx, (uint8_t *)in_ecx, in_edx);
            break;
        }
        case SYSCALL_WRITE: {  // write file
            ret = file_write((uint32_t)in_ebx, (uint8_t *)in_ecx, in_edx);
            //file_write((FILE *)in_ebx, (char *)in_ecx, in_edx);
            break;
        }
        case SYSCALL_IOCTL: {  // ioctl file
            ret = file_ioctl((uint32_t)in_ebx, (uint32_t)in_ecx, (uint32_t)in_edx);
            //file_ioctl((FILE *)in_ebx, in_ecx);
            break;
        }
        case SYSCALL_MALLOC: {
            ret = (uint32_t)kmalloc(in_ebx);
            break;
        }
        case SYSCALL_FREE: {
            kfree((void *)in_ebx);
            break;
        }
        case SYSCALL_TEST: {  // test syscalls works, just sum first and second params
            printf(MSG_SYSCALL_TEST);
            ret = in_ebx + in_ecx;
            break;
        }
        default:  // do nothing.
            break;
    }
    enable_int();
    return ret;
}
