#include <kernel/DT/dt.h>
#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define __MODULE_NAME__ "SCHED"

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
    // or not
    __asm__("int $0x20");  // launch scheduler
}

void ksend(uint16_t tid, struct message_t* msg) {
    struct task_t* task = task_find_by_id(tid);
    if (!task || task->msg_count_in == TASK_MSG_BUFF_SIZE) {
        klog("Task id %x not found or his pool full\n", tid);
        return;
    }
    memcpy(&task->msg_buff[task->msg_count_in], msg, sizeof(struct message_t));
    task->msg_count_in++;
    if (task->status == TASK_INTERRUPTABLE)
        task->status = TASK_RUNNING;
}

void krecive(uint16_t tid, struct message_t* msg) {
    struct task_t* task_before;
    struct task_t* task_after;

    task_before = current_task;
    assert(tid == task_before->tid);
    assert(task_before->status == TASK_RUNNING);

    if (task_before->msg_count_in == 0) {
        task_before->status = TASK_INTERRUPTABLE;
        sched_yield();
    }
    task_after = current_task;

    assert(task_after == task_before);
    assert(task_after->status == TASK_RUNNING);

    memcpy(msg, &task_after->msg_buff[task_after->msg_count_in - 1], sizeof(struct message_t));
    task_after->msg_count_in--;
}
