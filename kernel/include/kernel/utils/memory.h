#include <types.h>
#include <kernel/defines.h>

#ifndef KERNEL_UTILS_MEMORY_H_
#define KERNEL_UTILS_MEMORY_H_

// one page is 4kb
// one table is 4mb
// VIRT_BASE is 3gb
#define DIRECTORY_BIT_FIELD     (0b11111111110000000000000000000000)
#define TABLE_BIT_FIELD         (0b00000000001111111111000000000000)
#define PDTE_BIT_FIELD          (DIRECTORY_BIT_FIELD | TABLE_BIT_FIELD)

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


struct mem_info_t {
    uint16_t free_mem_in_page;
    void *pointer;
};

extern size_t boot_page_directory;
extern size_t boot_page_table;

extern volatile uint32_t *page_directory_ptr;
extern volatile uint32_t *page_table_ptr;

extern size_t last_page_ID;

extern uint8_t last_mem_page_id;
extern struct mem_info_t allocated_pages[128];

extern void init_memory_manager();

extern void* alloc_page();
extern void free_page(void *page_addr_in);

extern void* kmalloc(size_t count);
extern void kfree(void *ptr);

#endif
