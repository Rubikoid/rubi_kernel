#include <kernel/kthread/dq.h>
#include <kernel/scheduler/task.h>
#include <kernel/DT/syscall.h>


void dq_task() {
    struct message_t msg;

    for (;;) {
        syscall_krecv(&msg);

        switch (msg.type) {
            case IPC_MSG_TYPE_DQ_SCHED:
                /* do deffered callback execution */
                // assert(msg.len == 4);
                /*dq_handler_t handler = (dq_handler_t)*((size_t*)msg.data);
                assert((size_t)handler < KERNEL_CODE_END_ADDR);
                printf(MSG_DQ_SCHED, handler);
                handler(msg);*/
                break;
        }
    }

    syscall_exit();
}
