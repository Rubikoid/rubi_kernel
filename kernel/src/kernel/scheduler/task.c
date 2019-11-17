#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/scheduler/task.h>
#include <kernel/utils/DT/dt.h>
#include <lib/stdio.h>
#include <lib/string.h>

struct clist_def_t task_list = {
    .slot_size = sizeof(struct task_t),
    .slots = 0,
    .head = 0,
};
struct task_t* current_task = 0;

void task_create(uint16_t tid, void* start_addr, struct task_mem_t* task_mem) {
    struct task_t* task;
    struct clist_head_t* entry;

    entry = clist_insert_after(&task_list, task_list.head);
    task = (struct task_t*)entry->data;

    task->kstack = kmalloc(4096);
    task->tid = tid;
    task->name[0] = 'F';
    task->name[1] = '\0';
    task->status = TASK_UNINTERRUPTABLE;
    memcpy(&task->task_mem, task_mem, sizeof(struct task_mem_t));

    *(uint32_t*)(&task->flags) = get_eflags() | 0x200;  // enabled interrupts
    memset(&task->gp_registers, 0, sizeof(struct gp_registers_t));

    task->op_registers.cs = GDT_KCODE_SELECTOR;
    task->op_registers.ds = GDT_KDATA_SELECTOR;
    task->op_registers.ss = GDT_KSTACK_SELECTOR;

    task->op_registers.eip = (size_t)start_addr;
    task->op_registers.cr3 = (size_t)task->task_mem.page_dir;
    task->op_registers.k_esp = (uint32_t)task->kstack + 4096;
    printf("Task tid: %u, eip:%x, stack:%x\n", tid, (size_t)start_addr, task->kstack);
}

void task_delete(struct task_t* task) {
    if (task == NULL)
        return;

    kfree(task->kstack);
    task->kstack = NULL;

    if (task->task_mem.pages_count > 0) {
        for (int i = 0; i < task->task_mem.pages_count; i++)
            unbind_page(task->task_mem.page_table, (size_t)(task->task_mem.pages + i * PAGE_SIZE));
    }
    unbind_table(task->task_mem.page_dir, 0x0);
    free_page_table(task->task_mem.page_table);
    free_page_directory(task->task_mem.page_dir);

    clist_delete(&task_list, (struct clist_head_t*)task);
}
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

        if (current_task->time < TASK_QUOTA && !current_task->reschedule)
            return;  // continue

        current_task->time = 0;
        current_task->reschedule = 0;

        current_task->op_registers.eip = *ret_addr;
        current_task->op_registers.cs = *(uint16_t*)((size_t)ret_addr + 4);
        *(uint32_t*)(&current_task->flags) = *(uint32_t*)((size_t)ret_addr + 6) | 0x200;
        current_task->op_registers.k_esp = (size_t)ret_addr + 12;
        current_task->gp_registers.esp = current_task->op_registers.k_esp;
        memcpy(&current_task->gp_registers, (void*)reg_addr, sizeof(struct gp_registers_t));
    }
    current_task = (current_task == NULL) ? (struct task_t*)task_list.head : current_task;
    if (current_task == NULL)
        return;

    for (int i = 0; i <= task_list.slots; i++) {
        next_task = (struct task_t*)current_task->list_head.next;
        if (next_task->status == TASK_RUNNING)
            break;
    }

    if (current_task && current_task->status == TASK_KILLING)
        task_delete(current_task);
    else {
        struct task_t* task = (struct task_t*)task_list.head;
        for (int i = 0; i <= task_list.slots; i++) {
            task = (struct task_t*)task->list_head.next;
            if (task->status == TASK_KILLING)
                break;
        }
        if (task && task->status == TASK_KILLING)
            task_delete(current_task);
    }

    next_task->op_registers.k_esp -= 4;
    *(uint32_t*)(next_task->op_registers.k_esp) = (*(uint16_t*)(&next_task->flags)) | 0x200;
    next_task->op_registers.k_esp -= 4;
    *(uint32_t*)(next_task->op_registers.k_esp) = next_task->op_registers.cs;
    next_task->op_registers.k_esp -= 4;
    *(uint32_t*)(next_task->op_registers.k_esp) = next_task->op_registers.eip;
    next_task->gp_registers.esp = next_task->op_registers.k_esp;
    next_task->op_registers.k_esp -= sizeof(struct gp_registers_t);
    memcpy((void*)next_task->op_registers.k_esp,
           (void*)&next_task->gp_registers, sizeof(struct gp_registers_t));

    current_task = next_task;
    printf("SWT tid: %u, esp: %x, ret: %x, eip: %x\n", next_task->tid, next_task->op_registers.k_esp, *ret_addr, next_task->op_registers.eip);
    // switch_kcontext(next_task->op_registers.k_esp, next_task->op_registers.cr3);
}

void sched_yield() {
    if (current_task)
        current_task->reschedule = 1;
    __asm__("int $0x20");  // launch scheduler
}
