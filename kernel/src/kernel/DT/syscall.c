#include <types.h>

#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

#define __MODULE_NAME__ "SCALL"

static const char *syscall_names[NORMAL_SYSCALL_COUNT] = {
    "SYSCALL_NONE",
    "SYSCALL_EXIT",
    "SYSCALL_KSEND",
    "SYSCALL_KRECV",
    "SYSCALL_GETTID",
};

uint32_t cint_syscall(PUSHAD_C) {
    struct task_t *task = current_task;
    klog(MSG_SYSCALL, syscall_names[in_eax % NORMAL_SYSCALL_COUNT], in_eax, task == NULL ? ("NON") : (task->name), task == NULL ? 9999 : task->tid);
    uint32_t ret = 0;
    disable_int();
    switch (in_eax) {
        case SYSCALL_EXIT: {
            printf(MSG_SYSCALL_EXIT, task == NULL ? 9999 : task->tid);
            current_task->status = TASK_KILLING;
            sched_yield();
            ret = 0;
            break;
        }
        case SYSCALL_KSEND: {
            ksend(in_ebx, (struct message_t *)in_ecx);
            break;
        }
        case SYSCALL_KRECV: {
            krecive(task->tid, (struct message_t *)in_ebx);
            break;
        }
        case SYSCALL_GETTID: {
            ret = (uint32_t)task->tid;  // tid - 16, return - 32. вдруг что сломается?
            break;
        }
        case SYSCALL_TEST: {
            printf(MSG_SYSCALL_TEST);
            ret = in_ebx + in_ecx;
            break;
        }
        default:
            break;
    }
    enable_int();
    return ret;
}
