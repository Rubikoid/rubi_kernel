#include <kernel/DT/dt.h>
#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <lib/stdio.h>
#include <lib/string.h>
/*
/|\  ...
 |   eip
 |   ret_addr
 |   reg_addr
*/
void sched_schedule(size_t* ret_addr, size_t* reg_addr) {
    struct task_t* next_task = NULL;
    if (current_task != NULL) {
        current_task->time += 1;
        if (current_task->time < TASK_QUOTA && !current_task->reschedule) {
            printf(MSG_SCH_TC);
            return;  // continue
        }
        printf(MSG_SCH_TSW_SHORT);
        current_task->time = 0;
        current_task->reschedule = 0;

        current_task->op_registers.eip = *ret_addr;
        current_task->op_registers.cs = *(uint16_t*)((size_t)ret_addr + 4);
        *(uint32_t*)(&current_task->flags) = *(uint32_t*)((size_t)ret_addr + 6) | 0x200;
        // get esp from interrupt instead of getting esp from registers.
        // idk, how it works before.
        current_task->op_registers.u_esp = (size_t)ret_addr + 12; 
        memcpy(&current_task->gp_registers, (void*)reg_addr, sizeof(struct gp_registers_t));
        //current_task->op_registers.u_esp = current_task->gp_registers.esp;
    }

    if (current_task)
        next_task = task_find_by_status_from(current_task, TASK_RUNNING);
    else
        next_task = task_find_by_status(TASK_RUNNING);
    if (next_task == NULL)
        return;

    if (current_task && current_task->status == TASK_KILLING) {
        task_delete(current_task);
    } else {
        struct task_t* task = task_find_by_status(TASK_KILLING);
        if (task && task->status == TASK_KILLING) {
            task_delete(task);
        }
    }

    next_task->op_registers.u_esp -= 4;
    *(uint32_t*)(next_task->op_registers.u_esp) = (*(uint16_t*)(&next_task->flags)) | 0x200;
    next_task->op_registers.u_esp -= 4;
    *(uint32_t*)(next_task->op_registers.u_esp) = next_task->op_registers.cs;
    next_task->op_registers.u_esp -= 4;
    *(uint32_t*)(next_task->op_registers.u_esp) = next_task->op_registers.eip;
    next_task->gp_registers.esp = next_task->op_registers.u_esp;
    next_task->op_registers.u_esp -= sizeof(struct gp_registers_t);
    memcpy((void*)next_task->op_registers.u_esp, (void*)&next_task->gp_registers, sizeof(struct gp_registers_t));

    printf(MSG_SCH_TSW, next_task->tid, *ret_addr, next_task->op_registers.u_esp, next_task->op_registers.eip, &reg_addr);
    current_task = next_task;
    switch_kcontext(next_task->op_registers.u_esp, next_task->op_registers.cr3);
    assert(1 == 0);  // unreacheable
}

void sched_yield() {
    if (current_task)
        current_task->reschedule = 1;
    // dangerous place, here we can leak memory
    __asm__("int $0x20");  // launch scheduler
}
