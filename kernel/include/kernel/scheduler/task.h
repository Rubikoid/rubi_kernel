#include <kernel/defines.h>
#include <kernel/memory/mmu.h>
#include <lib/clist.h>
#include <types.h>

#ifndef KERNEL_SCHEDULER_TASK_H_
#define KERNEL_SCHEDULER_TASK_H_

#define TASK_UNINTERRUPTABLE 0
#define TASK_RUNNING 1
#define TASK_KILLING 2
#define TASK_INTERRUPTABLE 3

#define TASK_KSTACK_SIZE 1024
#define TASK_USTACK_SIZE 4096 - 1024

struct __attribute__((__packed__)) gp_registers_t {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp; /* current stack */
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
};

struct __attribute__((__packed__)) op_registers_t {
    uint32_t eip;
    uint32_t cr3;   /* page directory */
    uint32_t k_esp; /* kernel stack */
    uint32_t u_esp; /* user stack */
    uint16_t cs;
    uint16_t ds;
    uint16_t ss;
};

struct __attribute__((__packed__)) flags_t {
    uint8_t cflag : 1; /* carry flag */
    uint8_t res1 : 1;  /* reserved */
    uint8_t pflag : 1; /* parity flag */
    uint8_t res2 : 1;  /* reserved */
    uint8_t aflag : 1; /* auxiliary carry flag */
    uint8_t res3 : 1;  /* reserved */
    uint8_t zflag : 1; /* zero flag */
    uint8_t sflag : 1; /* sign flag */
    uint8_t tflag : 1; /* trap flag */
    uint8_t iflag : 1; /* interrupt enable flag */
    uint8_t dflag : 1; /* direction flag */
    uint8_t oflag : 1; /* overflow flag */
    uint8_t iopl : 2;  /* io privilege level */
    uint8_t nt : 1;    /* nested task */
    uint8_t res4 : 1;  /* reserved */

    uint8_t rflag : 1;   /* resume flag */
    uint8_t vmflag : 1;  /* virtual mode flag */
    uint8_t acflag : 1;  /* alignment check flag */
    uint8_t viflag : 1;  /* virtual interrupt flag */
    uint8_t vipflag : 1; /* virtual interrupt pending flag */
    uint8_t idflag : 1;  /* id flag */
    uint16_t res5 : 10;  /* reserved */
};

struct task_mem_t {
    void* pages;                             /* task physical pages */
    uint32_t pages_count;                    /* task physical pages count */
    struct page_directory_entry_t* page_dir; /* page directory */
    struct page_table_entry_t* page_table;   /* page table */
};

struct message_t {
    uint8_t type;  /* message type */
    uint32_t len;  /* data length */
    uint8_t* data; /* message data; reciver MUST clean data */
};

struct __attribute__((__packed__, __aligned__(4))) task_t {
    struct clist_head_t list_head;                 /* should be at first */
    uint16_t tid;                                  /* task id */
    char name[8];                                  /* task name */
    struct gp_registers_t gp_registers;            /* general purpose registers */
    struct op_registers_t op_registers;            /* other purpose registers */
    struct flags_t flags;                          /* processor flags */
    uint32_t time;                                 /* time of task execution */
    uint8_t reschedule;                            /* whether task need to be rescheduled */
    uint16_t status;                               /* task status */
    int msg_count_in;                              /* count of incomming messages */
    struct message_t msg_buff[TASK_MSG_BUFF_SIZE]; /* task message buffer */
    void* kstack;                                  /* kernel stack top */
    void* ustack;                                  /* user stack top */
    struct task_mem_t task_mem;                    /* task memory */
};

extern struct clist_def_t task_list;
extern struct task_t* current_task;
extern uint32_t tid_counter;  // TODO: do it normal

extern struct task_t* task_create(uint16_t tid, void* address, struct task_mem_t* task_mem, char* name);  // creates new task. ignores tid param. if task_mem=null - uses kernel memory context;
extern struct task_t* task_find_by_status(uint16_t status);
extern struct task_t* task_find_by_status_from(struct task_t* start, uint16_t status);
extern struct task_t* task_find_by_id(uint16_t tid);
extern void task_delete(struct task_t* task);

extern void sched_schedule(size_t* ret_addr, size_t* reg_addr);
extern void sched_yield();

extern void ksend(uint16_t tid, struct message_t* msg);    // kernel internal func to send IPC
extern void krecive(uint16_t tid, struct message_t* msg);  // kernel internal func to recv IPC

extern void tasks_debug();

#endif
