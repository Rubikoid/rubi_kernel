#include <kernel/scheduler/task.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_KTHREAD_KTASKS_H_
#define KERNEL_KTHREAD_KTASKS_H_

#define KERNEL_DQ_TASK_ID 0

extern uint32_t ktasks_size;
extern struct task_t **ktasks;

#endif
