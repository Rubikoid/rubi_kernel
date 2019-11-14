#include <kernel/defines.h>
#include <lib/slist.h>
#include <types.h>

#ifndef KERNEL_MEMORY_MMU_H_
#define KERNEL_MEMORY_MMU_H_

// one page is 4kb
// one table is 4mb
// VIRT_BASE is 3gb
#define DIRECTORY_BIT_FIELD (0b11111111110000000000000000000000)
#define TABLE_BIT_FIELD (0b00000000001111111111000000000000)
#define PDTE_BIT_FIELD (DIRECTORY_BIT_FIELD | TABLE_BIT_FIELD)
#define KERNEL_LOWER_TABLES 4
#define KERNEL_HIGHER_TABLES 4

struct __attribute__((__packed__)) page_directory_entry_t {
    uint8_t present : 1;
    uint8_t read_write : 1;
    uint8_t user_supervisor : 1;
    uint8_t write_through : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t zero : 1;
    uint8_t page_size : 1;
    uint8_t ignored : 1;
    uint8_t available : 3;
    uint32_t page_table_addr : 20;
};

struct __attribute__((__packed__)) page_table_entry_t {
    uint8_t present : 1;
    uint8_t read_write : 1;
    uint8_t user_supervisor : 1;
    uint8_t write_through : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t zero : 1;
    uint8_t global : 1;
    uint8_t available : 3;
    uint32_t page_phys_addr : 20;
};

// initial paging data
extern size_t boot_page_directory;
extern size_t boot_page_table;

extern volatile struct page_directory_entry_t *kernel_page_directory;
extern volatile struct page_table_entry_t *kernel_page_table;

extern void init_memory_manager();
extern void mmu_dump();

extern struct page_directory_entry_t *create_page_directory();
extern struct page_table_entry_t *create_page_table(size_t count);

extern void free_page_directory(struct page_directory_entry_t *pd);
#endif
