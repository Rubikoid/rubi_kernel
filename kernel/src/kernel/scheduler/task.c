#include <kernel/DT/dt.h>
#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vfs/file.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define __MODULE_NAME__ "TASK"

struct clist_def_t task_list = {
    .slot_size = sizeof(struct task_t),
    .slots = 0,
    .head = 0,
};
struct task_t* current_task = 0;
uint32_t tid_counter = 0;

struct task_t* task_create(uint16_t tid_depricated, void* start_addr, struct task_mem_t* task_mem, char* name) {
    struct task_t* task;
    struct clist_head_t* entry;

    entry = clist_insert_after(&task_list, task_list.tail);
    task = (struct task_t*)entry->data;
    // memset(task, 0, sizeof(struct task_t));

    /* init different task vars */
    task->kstack = kmalloc(TASK_KSTACK_SIZE);
    task->ustack = kmalloc(TASK_USTACK_SIZE);
    task->tid = tid_counter++;
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->status = TASK_UNINTERRUPTABLE;
    task->msg_count_in = 0;

    /* init task IO */
    task->next_fd = 0;
    task->fd_table.slot_size = sizeof(struct file_t);
    task->fd_table.slots = 0;
    task->fd_table.head = 0;

    task->fs_status.pwd = 0;
    task->fs_status.root = 0;

    /* init task mem */
    if (task_mem != NULL) {
        memcpy(&task->task_mem, task_mem, sizeof(struct task_mem_t));
    } else {  // if no memory provided, we think this is a kernel task and just use KPD/KPT
        struct task_mem_t t = {
            .pages = 0,
            .pages_count = 0,
            .page_dir = kernel_page_directory,
            .page_table = kernel_page_table,
        };
        memcpy(&task->task_mem, &t, sizeof(struct task_mem_t));
    }

    /* initializa task registers */
    *(uint32_t*)(&task->flags) = get_eflags() | 0x200;  // enabled interrupts
    memset(&task->gp_registers, 0, sizeof(struct gp_registers_t));

    task->op_registers.cs = GDT_KCODE_SELECTOR;
    task->op_registers.ds = GDT_KDATA_SELECTOR;
    task->op_registers.ss = GDT_KSTACK_SELECTOR;

    task->op_registers.eip = (size_t)start_addr;
    task->op_registers.cr3 = PHYS((size_t)task->task_mem.page_dir);
    task->op_registers.k_esp = (uint32_t)task->kstack + TASK_KSTACK_SIZE;
    task->op_registers.u_esp = (uint32_t)task->ustack + TASK_USTACK_SIZE;

    klog(MSG_TASK_CREATE, task->tid, task->op_registers.k_esp, task->op_registers.u_esp, (size_t)start_addr, task->name);
    return task;
}

void task_delete(struct task_t* task) {
    if (task == NULL)
        return;
    int tid = task->tid;
    kfree(task->kstack);
    kfree(task->ustack);

    task->kstack = NULL;
    task->ustack = NULL;

    if (task->task_mem.pages_count > 0) {
        for (int i = 0; i < task->task_mem.pages_count; i++)
            unbind_page(task->task_mem.page_table, (size_t)(task->task_mem.pages[i]), simple_page_bitmap_callback);
        kfree(task->task_mem.pages);
    }
    unbind_table(task->task_mem.page_dir, 0x0);  // WTF: what a magic constant
    free_page_table(task->task_mem.page_table);
    free_page_directory(task->task_mem.page_dir);

    clist_delete(&task_list, (struct clist_head_t*)task);
    // klog("Task killing tid=%d OK\n", tid);
}

struct task_t* task_find_by_status(uint16_t status) {
    return task_list.head != NULL ? task_find_by_status_from((struct task_t*)task_list.head->data, status) : NULL;
}

struct task_t* task_find_by_status_from(struct task_t* start, uint16_t status) {
    if (start == NULL) {
        klog("Start is null\n");
        return NULL;
    }
    struct task_t* work = (struct task_t*)start->list_head.next->data;
    do {
        if (work->status == status)
            return work;
        work = (struct task_t*)work->list_head.next->data;
    } while (work != start && work != NULL);
    return NULL;
}
struct task_t* task_find_by_id(uint16_t tid) {
    struct task_t* start = (struct task_t*)task_list.head->data;
    do {
        if (start->tid == tid)
            return start;
        start = (struct task_t*)start->list_head.next->data;
    } while (start != (struct task_t*)task_list.head->data && start != NULL);
    return NULL;
}

void tasks_debug() {
    struct task_t* task = (struct task_t*)task_list.head->data;
    klog("Tasks slots: %x\n", task_list.slots);
    if (task == NULL)
        return;
    do {
        klog("Task tid:%x, name:%s, state:%x time:%x, resched:%x\n", task->tid, task->name, task->status, task->time, task->reschedule);
        task = (struct task_t*)task->list_head.next->data;
    } while (task != (struct task_t*)task_list.head->data && task != NULL);
}
