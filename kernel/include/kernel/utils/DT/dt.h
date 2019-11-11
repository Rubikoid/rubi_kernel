#include <types.h>

#ifndef KERNEL_UTILS_DT_DT_H_
#define KERNEL_UTILS_DT_DT_H_

#define GDT_ENTRIES_COUNT 8
#define IDT_ENTRIES_COUNT 256

#define GDT_NULL_SEGMENT    0
#define GDT_KCODE_SEGMENT   1
#define GDT_KDATA_SEGMENT   2
#define GDT_KSTACK_SEGMENT  3
#define GDT_UCODE_SEGMENT   4
#define GDT_UDATA_SEGMENT   5
#define GDT_USTACK_SEGMENT  6
#define GDT_TSS_SEGMENT     7

#define GDT_NULL_SELECTOR   0x8*GDT_NULL_SEGMENT
#define GDT_KCODE_SELECTOR  0x8*GDT_KCODE_SEGMENT
#define GDT_KDATA_SELECTOR  0x8*GDT_KDATA_SEGMENT
#define GDT_KSTACK_SELECTOR 0x8*GDT_KSTACK_SEGMENT
#define GDT_KTSS_SELECTOR   0x8*GDT_TSS_SEGMENT
#define GDT_UCODE_SELECTOR  0x8*GDT_UCODE_SEGMENT
#define GDT_UDATA_SELECTOR  0x8*GDT_UDATA_SEGMENT
#define GDT_USTACK_SELECTOR 0x8*GDT_USTACK_SEGMENT
#define GDT_UTSS_SELECTOR   0x8*GDT_TSS_SEGMENT

#define INT_DIVIDE_BY_ZERO      0x0
#define INT_NMI                 0x2
#define INT_BREAKPOINT          0x3
#define INT_OVERFLOW            0x4
#define INT_BOUND               0x5
#define INT_OPCODE              0x6
#define INT_DEV_NOT_AVAIL       0x7
#define INT_DOUBLE_FAULT        0x8
#define INT_COPROC_SEGMENT_OVER 0x9
#define INT_INVALID_TSS         0xA
#define INT_SEGMENT_NOT_PRES    0xB
#define INT_SS_FAULT            0xC
#define INT_GENERAL_PROTECT     0xD
#define INT_PAGE_FAULT          0xE
#define INT_FPU                 0x10
#define INT_ALIGNMENT_CHECK     0x11

// #define INT_SYSCALL 0x80

#define INTERRUPT_GATE 0x8e


struct __attribute__((__packed__)) GDT_raw_entry_t {
    uint16_t limit_low: 16;                                                                     // 0,1
    uint16_t base_low: 16;                                                                      // 2,3
    uint8_t base_middle: 8;                                                                     // 4
    uint8_t type: 4; /* whether code (0b1010), data (0b0010), stack (0b0110) or tss (0b1001) */ // 5
    uint8_t s: 1; /* whether system descriptor */                                               // 5
    uint8_t dpl: 2; /* privilege level */                                                       // 5
    uint8_t p: 1; /* whether segment prensent */                                                // 5
    uint8_t limit_high: 4;                                                                      // 6
    uint8_t a: 1; /* reserved for operation system */                                           // 6
    uint8_t zero: 1; /* zero */                                                                 // 6
    uint8_t db: 1; /* whether 16 or 32 segment */                                               // 6
    uint8_t g: 1; /* granularity */                                                             // 6
    uint8_t base_high: 8;                                                                       // 7
};

struct GDT_normal_entry_t
{
    uint32_t base;
    uint32_t limit;
    uint8_t type: 4;
    
    uint8_t s: 1; /* whether system descriptor */
    uint8_t dpl: 2; /* privilege level */
    uint8_t p: 1; /* whether segment prensent */
    
    uint8_t a: 1; /* reserved for operation system */
    uint8_t db: 1; /* whether 16 or 32 segment */
    uint8_t g: 1; /* granularity */
};


struct __attribute__((__packed__)) GDT_pointer_t {
    uint16_t limit;
    uint32_t base;
};

struct __attribute__((__packed__)) IDT_entry_t {
    uint16_t offset_lowerbits;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_higherbits;
};

extern struct GDT_raw_entry_t gdt[GDT_ENTRIES_COUNT];
extern struct GDT_normal_entry_t normal_gdt[GDT_ENTRIES_COUNT];
extern struct GDT_pointer_t gdt_ptr;

extern struct IDT_entry_t idt_table[IDT_ENTRIES_COUNT];


extern void gdt_init();
extern void gdt_flush();
extern void encode_gdt_entry(struct GDT_raw_entry_t *target, struct GDT_normal_entry_t *source);

extern void idt_init();
extern void idt_fill_entry(uint8_t offset, size_t handler);

#endif
