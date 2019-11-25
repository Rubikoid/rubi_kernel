#include <kernel/DT/dt.h>
#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <lib/stdio.h>
#include <lib/string.h>

struct clist_def_t task_list = {
    .slot_size = sizeof(struct task_t),
    .slots = 0,
    .head = 0,
};
struct task_t* current_task = 0;

struct task_t* task_create(uint16_t tid, void* start_addr, struct task_mem_t* task_mem) {
    struct task_t* task;
    struct clist_head_t* entry;

    entry = clist_insert_after(&task_list, task_list.head);
    task = (struct task_t*)entry->data;

    task->kstack = kmalloc(TASK_KSTACK_SIZE);
    task->ustack = kmalloc(TASK_USTACK_SIZE);
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
    task->op_registers.cr3 = PHYS((size_t)task->task_mem.page_dir);
    task->op_registers.k_esp = (uint32_t)task->kstack + TASK_KSTACK_SIZE;
    task->op_registers.u_esp = (uint32_t)task->ustack + TASK_USTACK_SIZE;
    printf(MSG_TASK_CREATE, tid, task->op_registers.k_esp, task->op_registers.k_esp, (size_t)start_addr);
    return task;
}

void task_delete(struct task_t* task) {
    if (task == NULL)
        return;

    kfree(task->kstack);
    kfree(task->ustack);
    task->kstack = NULL;
    task->ustack = NULL;

    if (task->task_mem.pages_count > 0) {
        for (int i = 0; i < task->task_mem.pages_count; i++)
            unbind_page(task->task_mem.page_table, (size_t)(task->task_mem.pages + i * PAGE_SIZE));
    }
    unbind_table(task->task_mem.page_dir, 0x0);
    free_page_table(task->task_mem.page_table);
    free_page_directory(task->task_mem.page_dir);

    clist_delete(&task_list, (struct clist_head_t*)task);
}

struct task_t* task_find_by_status(uint16_t status) {
    return task_find_by_status_from((struct task_t*)task_list.head, status);
}
struct task_t* task_find_by_status_from(struct task_t* start, uint16_t status) {
    if (start == NULL)
        return NULL;
    for (int i = 0; i <= task_list.slots; i++) {
        if (start->status == status)
            return start;
        start = (struct task_t*)start->list_head.next;
    }
    return start;
}
struct task_t* task_find_by_id(uint16_t tid) {
    struct task_t* start = (struct task_t*)task_list.head;
    for (int i = 0; i <= task_list.slots; i++) {
        if (start->tid == tid)
            return start;
        start = (struct task_t*)start->list_head.next;
    }
    return 0;
}
