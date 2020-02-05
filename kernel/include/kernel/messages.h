#include <types.h>

#ifndef KERNEL_MESSAGES_H_
#define KERNEL_MESSAGES_H_

#ifdef DEBUG_INFINITY
#define MSG_INFINITY "{inf}\n"
#else
#define MSG_INFINITY ""
#endif

#define MSG_TASK_CREATE "Task create tid: %u, k_esp:%x, u_esp:%x, eip:%x, name: %s\n"

#ifdef DEBUG_SCHED
#define MSG_SCH_TC "{tcont}\n"
#define MSG_SCH_TSW_SHORT "{tsw}\n"
#define MSG_SCH_TSW "Tsw to tid: %u, from: %x esp: %x, eip: %x, kernel_esp: %x\n"
#define MSG_SCH_TSW_F "Tsw from tid: %u, from: %x esp: %x, eip: %x, kernel_esp: %x\n"
#define MSG_SCH_UPTIME "uptime_secs: %u\n"
#else
#define MSG_SCH_TC ""
#define MSG_SCH_TSW_SHORT ""
#define MSG_SCH_TSW ""
#define MSG_SCH_TSW_F ""
#define MSG_SCH_UPTIME "uptime_secs: %u\n"
#endif

#define MSG_SYSCALL "Syscall %x from %x\n"
#define MSG_SYSCALL_EXIT "Task killing tid: %u\n"
#define MSG_SYSCALL_TEST "{tsyscall}\n"

#endif
