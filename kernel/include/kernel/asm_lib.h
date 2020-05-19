#include <types.h>

#ifndef KERNEL_ASMLIB_H_
#define KERNEL_ASMLIB_H_

#define MULTIBOOT_FLAG_MEM     0x001
#define MULTIBOOT_FLAG_DEVICE  0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS    0x008
#define MULTIBOOT_FLAG_AOUT    0x010
#define MULTIBOOT_FLAG_ELF     0x020
#define MULTIBOOT_FLAG_MMAP    0x040
#define MULTIBOOT_FLAG_CONFIG  0x080
#define MULTIBOOT_FLAG_LOADER  0x100
#define MULTIBOOT_FLAG_APM     0x200
#define MULTIBOOT_FLAG_VBE     0x400

struct __attribute__((__packed__)) mod_addr_t {
    uint32_t start;
    uint32_t end;
};

struct __attribute__((__packed__)) multiboot_t {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    struct mod_addr_t *mods_addr;
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
};

extern void disable_int();
extern void enable_int();
extern void halt();

extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);

extern void outw(uint16_t port, uint16_t value);
extern uint16_t inw(uint16_t port);

extern void outdw(uint16_t port, uint32_t value);
extern uint32_t indw(uint16_t port);

extern void gdt_load(void *gdt_ptr);
extern void idt_load(unsigned long *addr);

extern void enable_paging(void *page_directory);

extern void *get_cr3();
extern uint32_t get_eflags(void);

extern void switch_kcontext(uint32_t esp, uint32_t cr3);
// extern uint32_t fsyscall(uint32_t id, uint32_t arg1, uint32_t arg2, uint32_t arg3);

extern void ddd();
#endif
