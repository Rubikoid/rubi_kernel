#include <kernel/DT/syscall.h>
#include <kernel/kthread/dq.h>
#include <kernel/memory/heap.h>
#include <kernel/scheduler/task.h>

#define __MODULE_NAME__ "KDQ"

void dq_task() {
    struct message_t msg;
    klog("KernelDQ start\n");
    for (;;) {
        syscall_krecv(&msg);
        switch (msg.type) {
            case IPC_MSG_TYPE_DQ_SCHED: {
                /* do deffered callback execution */
                assert(msg.len == 4);
                dq_handler_t handler = (dq_handler_t) * ((size_t*)msg.data);
                // printf(MSG_DQ_SCHED, handler);
                handler(&msg);
                break;
            }
            case IPC_MSG_TYPE_DQ_TEST: {
                if (msg.len == 2) {
                    uint16_t ret_tid = *((uint16_t*)msg.data);

                    struct message_t msg_send;
                    msg_send.type = 111;
                    msg_send.len = 4;
                    msg_send.data = kmalloc(msg_send.len);
                    *((uint32_t*)msg_send.data) = 1337;
                    syscall_ksend(ret_tid, &msg_send);
                }
                break;
            }
        }
        kfree(msg.data);
    }

    syscall_exit();
}
