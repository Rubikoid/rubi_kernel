#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL_UTILS_MEMORY_H_
#define KERNEL_UTILS_MEMORY_H_

// one page is 4kb
// one table is 4mb
// VIRT_BASE is 3gb
#define VIRT_BASE               (0xC0000000)
#define PAGE_SIZE               (0x1000)
#define TABLE_SIZE              (PAGE_SIZE*1024)
#define KERNEL_TABLE_COUNT      (4)

#define DIRECTORY_BIT_FIELD     (0b11111111110000000000000000000000)
#define TABLE_BIT_FIELD         (0b00000000001111111111000000000000)
#define PDTE_BIT_FIELD          (DIRECTORY_BIT_FIELD | TABLE_BIT_FIELD)

extern size_t boot_page_directory;
extern size_t boot_page_table;

extern volatile uint32_t *page_directory_ptr;
extern volatile uint32_t *page_table_ptr;

extern size_t last_page_ID;

void init_memory_manager();

void *alloc_page();
void free_page(void *page_addr_in);

void *alloc_mem(size_t count);
void free_mem(void *ptr);

#endif
