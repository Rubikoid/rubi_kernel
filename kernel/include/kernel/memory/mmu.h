#include <kernel/defines.h>
#include <lib/slist.h>
#include <types.h>

#ifndef KERNEL_MEMORY_MMU_H_
#define KERNEL_MEMORY_MMU_H_

// one page is 4kb
// one table is 4mb
// VIRT_BASE is 3gb
#define DIRECTORY_BIT_FIELD (0b11111111110000000000000000000000)
#define TABLE_BIT_FIELD     (0b00000000001111111111000000000000)
#define PDTE_BIT_FIELD      (DIRECTORY_BIT_FIELD | TABLE_BIT_FIELD)

#define KERNEL_LOWER_TABLES    4
#define KERNEL_HIGHER_TABLES   4
#define KERNEL_EXTENDED_TABLES 128
#define USER_SPACE_TABLES      16

#define KERNEL_PAGES_END      (VIRT_BASE + (KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE)
#define KERNEL_EXTENDED_END (KERNEL_PAGES_END + KERNEL_EXTENDED_TABLES * TABLE_SIZE)

#define PHYS_TASKS_SPACE_START ((KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE)
#define PHYS_TASKS_SPACE_END   ((KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE + 8 * TABLE_SIZE)

#define MM_BITMAP_TABLES KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES + USER_SPACE_TABLES

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

typedef void (*bitmap_callback_t)(uint32_t phys_addr, uint32_t linear_addr, uint32_t set);

// initial paging data
extern size_t boot_page_directory;
extern size_t boot_page_table;

extern struct page_directory_entry_t *kernel_page_directory;
extern struct page_table_entry_t *kernel_page_table;


// one bitmap int = 32 pages -> 32 bitmap ints is one table == 4 mb
// 32 is count of 32's bitmaps for one TABLE (one table == 1024 entry -> 1024/32)
// 16 is count of pages for 64 mb (64 mb / 4 mb per page = 16)

// VIRTUAL! pages bitmap. The mais purpose of extended kernel space is for PCI devices and some memory mappings.
extern uint32_t extended_kernel_pages_bitmap[32 * KERNEL_EXTENDED_TABLES];

// physical pages bitmap, we should't occure one page two times!
extern uint32_t pages_bitmap[32 * MM_BITMAP_TABLES];


extern void init_memory_manager();
extern void mmu_dump(struct page_directory_entry_t *pd);

extern struct page_directory_entry_t *create_page_directory();
extern struct page_table_entry_t *create_page_table(size_t count);  // count of pages!!

extern void *alloc_page(struct page_table_entry_t *pt, size_t liner_addr);
extern void bind_addr(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr, bitmap_callback_t bm_callb);
extern void bind_table(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr);  // you MUST bind every PT with PD using that func!!
extern void bind_page(struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr, bitmap_callback_t bm_callb);
extern void unbind_page(struct page_table_entry_t *pt, size_t liner_addr, bitmap_callback_t bm_callb);
extern void unbind_table(struct page_directory_entry_t *pd, size_t liner_addr);

extern void simple_page_bitmap_callback(uint32_t phys_addr, uint32_t linear_addr, uint32_t set);
extern void extended_kernel_bitmap_callback(uint32_t phys_addr, uint32_t linear_addr, uint32_t set);

extern void free_page_table(struct page_table_entry_t *pt);
extern void free_page_directory(struct page_directory_entry_t *pd);
#endif
