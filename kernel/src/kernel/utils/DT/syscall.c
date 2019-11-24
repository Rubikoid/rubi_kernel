#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/scheduler/task.h>
#include <kernel/utils/DT/int.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

uint32_t cint_syscall(PUSHAD_C) {
    struct task_t* task = current_task;
    printf("Syscall %x from %x\n", in_eax, task == NULL ? 9999 : task->tid);
    disable_int();
    switch (in_eax) {
        case 1: {
            printf("Task killing tid: %u\n", task == NULL ? 9999 : task->tid);
            current_task->status = TASK_KILLING;
            sched_yield();
            return 0;
            break;
        }
        case 2: {
            printf("Syscall 2 here");
            break;
        }
        default:
            break;
    }
    enable_int();
    return 0;
}
