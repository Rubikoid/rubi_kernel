#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vfs/file.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
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

    //klog(MSG_SYSCALL, syscall_names[in_eax % NORMAL_SYSCALL_COUNT], in_eax, task == NULL ? ("NON") : (task->name), task == NULL ? 9999 : task->tid);

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
            FILE *file = file_open((uint8_t *)in_ebx, (uint16_t)in_ecx);
            ret = (size_t)file;
            break;
        }
        case SYSCALL_CLOSE: {  // close file
            break;
        }
        case SYSCALL_READ: {  // read file
            ret = file_read((FILE *)in_ebx, (char *)in_ecx, in_edx);
            break;
        }
        case SYSCALL_WRITE: {  // write file
            file_write((FILE *)in_ebx, (char *)in_ecx, in_edx);
            break;
        }
        case SYSCALL_IOCTL: {  // ioctl file
            file_ioctl((FILE *)in_ebx, in_ecx);
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
