#include <kernel/kthread/dq.h>
#include <kernel/scheduler/task.h>
#include <kernel/DT/syscall.h>
#include <kernel/kthread/ktasks.h>
#include <kernel/memory/heap.h>
#include <kernel/kthread/dq.h>


uint32_t ktasks_size = 0;
struct task_t **ktasks = 0;

void kernel_tasks_init() {
    ktasks_size = 1;
    ktasks = kmalloc(ktasks_size * sizeof(struct task_t *));

    ktasks[KERNEL_DQ_TASK_ID] = task_create(0, dq_task, NULL);
    ktasks[KERNEL_DQ_TASK_ID]->status = TASK_RUNNING;
}