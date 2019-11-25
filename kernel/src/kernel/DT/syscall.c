#include <types.h>

#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

uint32_t cint_syscall(PUSHAD_C) {
    struct task_t *task = current_task;
    printf(MSG_SYSCALL, in_eax, task == NULL ? 9999 : task->tid);
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
