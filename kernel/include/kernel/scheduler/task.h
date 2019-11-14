#include <kernel/defines.h>
#include <kernel/memory/mmu.h>
#include <lib/slist.h>
#include <types.h>

#ifndef KERNEL_SCHEDULER_TASK_H_
#define KERNEL_SCHEDULER_TASK_H_

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

struct __attribute__((__packed__)) task_t {
    struct slist_head_t list_head;      /* should be at first */
    uint16_t tid;                       /* task id */
    char name[8];                       /* task name */
    struct gp_registers_t gp_registers; /* general purpose registers */
    struct op_registers_t op_registers; /* other purpose registers */
    struct flags_t flags;               /* processor flags */
    uint32_t time;                      /* time of task execution */
    uint8_t reschedule;                 /* whether task need to be rescheduled */
    uint16_t status;                    /* task status */
    int msg_count_in;                   /* count of incomming messages */
    // struct message_t msg_buff[10];      /* task message buffer */
    void* kstack;               /* kernel stack top */
    void* ustack;               /* user stack top */
    struct task_mem_t task_mem; /* task memory */
};

#endif
