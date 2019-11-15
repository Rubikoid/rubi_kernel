#include <types.h>

#ifndef KERNEL_ASMLIB_H_
#define KERNEL_ASMLIB_H_

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

extern void gdt_load(void *gdt_ptr);
extern void idt_load(unsigned long *addr);

extern void enable_paging(void *page_directory);
extern void *get_cr3();
#endif
