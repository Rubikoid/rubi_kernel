#include <kernel/scheduler/task.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_KTHREAD_DQ_H_
#define KERNEL_KTHREAD_DQ_H_

#define IPC_MSG_TYPE_DQ_SCHED 0
#define IPC_MSG_TYPE_DQ_TEST  1

typedef void (*dq_handler_t)(struct message_t *msg);

extern void dq_task();

#endif
