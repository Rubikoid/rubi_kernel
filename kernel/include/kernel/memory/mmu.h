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

#define KERNEL_LOWER_TABLES    4    // count of lower kernel tables. 16 MB
#define KERNEL_HIGHER_TABLES   4    // count of higher kernel tables. 16 MB
#define KERNEL_EXTENDED_TABLES 128  // count of extended kernel tables. 4 * 128 MB
#define USER_SPACE_TABLES      8    // count of user space tables. 32 MB

#define KERNEL_PAGES_END (VIRT_BASE + (KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE)  // end of kernel space in virtual space. about 3GB + 32 MB

#define KERNEL_EXTENDED_START (KERNEL_PAGES_END + PAGE_SIZE)                                 // start of ext kernel pages
#define KERNEL_EXTENDED_END   (KERNEL_EXTENDED_START + KERNEL_EXTENDED_TABLES * TABLE_SIZE)  // end of extended kernel pages. About 3GB + 32 MB + 512 MB

#define PHYS_TASKS_SPACE_START ((KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE)  // skip some physical pages indended for kernel
#define PHYS_TASKS_SPACE_END   ((KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE + USER_SPACE_TABLES * TABLE_SIZE)

#define MM_BITMAP_TABLES        (KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES + USER_SPACE_TABLES)  // Count of tables, which can be bitmapped
#define MM_BITMAP_INT_PER_TABLE 32                                                                // one bitmap int = 32 pages -> 32 bitmap int32s == 1 table == 4 mb

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
//extern struct page_table_entry_t *kernel_extended_page_table[KERNEL_EXTENDED_TABLES];

// 32 is count of 32's bitmaps for one TABLE (one table == 1024 entry -> 1024/32)
// 16 is count of pages for 64 mb (64 mb / 4 mb per page = 16)

// VIRTUAL! pages bitmap. The mais purpose of extended kernel space is for PCI devices and some memory mappings.
extern uint32_t extended_kernel_pages_bitmap[MM_BITMAP_INT_PER_TABLE * KERNEL_EXTENDED_TABLES];

// physical pages bitmap, we should't occure one page two times!
extern uint32_t pages_bitmap[MM_BITMAP_INT_PER_TABLE * MM_BITMAP_TABLES];

extern void init_memory_manager();
extern void mmu_dump(struct page_directory_entry_t *pd);

extern struct page_directory_entry_t *create_page_directory();
extern struct page_table_entry_t *create_page_table(size_t count);  // count of pages!!

// get free page from bitmap, according to liner_addr and reserve it in page table.
extern void *alloc_page(struct page_table_entry_t *pt, size_t liner_addr);

// get free page from !bitmap for extended pages!, according to !phys_addr! and reserve it in page table.
extern void *alloc_page_extended(struct page_table_entry_t *pt, size_t phys_addr);

// bind liner_addr to phys_addr, set bit in bm_callback, bind everything right with pd and pt
extern void bind_addr(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr, bitmap_callback_t bm_callb);

// bind table to directory for specified liner_addr. You MUST bind every PT with PD using that func!!
extern void bind_table(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr);

// bind page at liner_addr to phys_addr, write it to page table and set bit in bm_callback
extern void bind_page(struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr, bitmap_callback_t bm_callb);

// unbind page for liner_addr it table, reset bit in bm_callback
extern void unbind_page(struct page_table_entry_t *pt, size_t liner_addr, bitmap_callback_t bm_callb);

// unbind table for liner_addr from page directory
extern void unbind_table(struct page_directory_entry_t *pd, size_t liner_addr);

extern void simple_page_bitmap_callback(uint32_t phys_addr, uint32_t linear_addr, uint32_t set);
extern void extended_kernel_bitmap_callback(uint32_t phys_addr, uint32_t linear_addr, uint32_t set);

extern void free_page_table(struct page_table_entry_t *pt);
extern void free_page_directory(struct page_directory_entry_t *pd);

// TODO: make this inline for performance maybe?
extern uint8_t get_page_from_bitmap(uint32_t *bitmap, size_t addr);
extern void set_page_to_bitmap(uint32_t *bitmap, size_t addr, uint8_t value);

#endif
