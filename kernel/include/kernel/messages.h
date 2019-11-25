#include <types.h>

#ifndef KERNEL_MESSAGES_H_
#define KERNEL_MESSAGES_H_

#define MSG_INFINITY "{inf}\n"

#define MSG_TASK_CREATE "Task create tid: %u, k_esp:%x, u_esp:%x, eip:%x\n"

#define MSG_SCH_TC "{tcont}\n"
#define MSG_SCH_TSW_SHORT "{tsw}\n"
#define MSG_SCH_TSW "Tsw to tid: %u, from: %x esp: %x, eip: %x, kernel_esp: %x\n"
#define MSG_SCH_TSW_F "Tsw from tid: %u, from: %x esp: %x, eip: %x, kernel_esp: %x\n"

#define MSG_SYSCALL "Syscall %x from %x\n"
#define MSG_SYSCALL_EXIT "Task killing tid: %u\n"
#define MSG_SYSCALL_TEST "{tsyscall}\n"

#endif
