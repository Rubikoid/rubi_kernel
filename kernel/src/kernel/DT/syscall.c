#include <types.h>

#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

uint32_t cint_syscall(PUSHAD_C) {
    struct task_t* task = current_task;
    printf("Syscall %x from %x\n", in_eax, task == NULL ? 9999 : task->tid);
    uint32_t ret = 0;
    disable_int();
    switch (in_eax) {
        case SYSCALL_EXIT: {
            printf("Task killing tid: %u\n", task == NULL ? 9999 : task->tid);
            current_task->status = TASK_KILLING;
            sched_yield();
            ret = 0;
            break;
        }
        case SYSCALL_TEST: {
            printf("{tsyscall}");
            ret = in_ebx + in_ecx;
            break;
        }
        default:
            break;
    }
    enable_int();
    return ret;
}
