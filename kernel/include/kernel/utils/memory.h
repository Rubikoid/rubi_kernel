#include <types.h>
#include <kernel/defines.h>
#include <lib/slist.h>

#ifndef KERNEL_UTILS_MEMORY_H_
#define KERNEL_UTILS_MEMORY_H_

// one page is 4kb
// one table is 4mb
// VIRT_BASE is 3gb
#define DIRECTORY_BIT_FIELD     (0b11111111110000000000000000000000)
#define TABLE_BIT_FIELD         (0b00000000001111111111000000000000)
#define PDTE_BIT_FIELD          (DIRECTORY_BIT_FIELD | TABLE_BIT_FIELD)

#define KHEAP_SIZE 1024

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

struct kheap_entry_t {
	struct slist_head_t head;
	size_t addr;
	size_t size;
	uint8_t is_busy;
};

// padding data
extern size_t boot_page_directory;
extern size_t boot_page_table;

extern volatile struct page_directory_entry_t *kernel_page_directory;
extern volatile struct page_table_entry_t *kernel_page_table;

// kernel heap data
extern struct kheap_entry_t kheap_blocks[KHEAP_SIZE];
extern struct slist_def_t kheap_list;

extern void init_memory_manager();

extern void mmu_dump();
extern void* alloc_page();
extern void free_page(void *page_addr_in);

extern void init_kheap_pages();
extern void* kmalloc(size_t count);
extern void kfree(void *ptr);

extern void* kmalloc_a(size_t count, uint32_t align);
extern void kfree_a(void *ptr);

extern void kheap_dump(struct slist_def_t *list);
#endif
