#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <types.h>

struct page_directory_entry_t *kernel_page_directory;
struct page_table_entry_t *kernel_page_table;
uint32_t pages_bitmap[32 * 16];
uint8_t kernel_pages_operations = 0;
/*
+------------------------+------------------------+------+----------------------------------------------------+
|          PHYS          |         LINEAR         | SIZE |                      PURPOSE                       |
+------------------------+------------------------+------+----------------------------------------------------+
| 0x00000000->0x000FFFFF | 0xC0000000->0xC00FFFFF | 1MB  | bios, vga, etc                                     |
| 0x00100000->0x00FFFFFF | 0xC0100000->0xC0FFFFFF | 15MB | lower kernel code, kernel stack, kernel everything |
| 0x01000000->0x01FFFFFF | 0xC1000000->0xC1FFFFFF | 16MB | kernel heap                                        |
+------------------------+------------------------+------+----------------------------------------------------+
|          PHYS          |         LINEAR         | SIZE |                      PURPOSE                       |
+------------------------+------------------------+------+----------------------------------------------------+
| 0x02000000->0x03FFFFFF | 0x00000000->0x01FFFFFF | 32MB | user applications                                  |
| 0x04000000->0x05FFFFFF | 0x00000000->0x01FFFFFF | 32MB | user applications                                  |
| 0x06000000->0x07FFFFFF | 0x00000000->0x01FFFFFF | 32MB | user applications                                  |
+------------------------+------------------------+------+----------------------------------------------------+
    one page is 4kb
    one table is 4mb
    so one table contains 1024 pages;
    VIRT_BASE is 3gb
*/

// TODO: check for hardcoded addrs everywhere

void init_memory_manager() {
    kernel_page_directory = (struct page_directory_entry_t *)&boot_page_directory;
    kernel_page_table = (struct page_table_entry_t *)&boot_page_table;
    memset(pages_bitmap, 0, 32 * 16);
    /* first 16 mb of kernel */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 32; j++) {
            pages_bitmap[i * 32 + j] = 0b11111111111111111111111111111111;
        }
    }
    /* first 16 mb of kernel */
    int k = KERNEL_LOWER_TABLES * 1024;
    kernel_pages_operations = 1;
    for (size_t i = VIRT_BASE + KERNEL_LOWER_TABLES * TABLE_SIZE; i < VIRT_BASE + (KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE; i += PAGE_SIZE, k += 1) {
        if (i % TABLE_SIZE == 0) {
            bind_table(kernel_page_directory, kernel_page_table + k, i);
        }
        bind_page(kernel_page_table + (k & (TABLE_BIT_FIELD >> 2)), i, PHYS(i));
    }
    kernel_pages_operations = 0;
}

void mmu_dump(struct page_directory_entry_t *pd) {
    //struct page_directory_entry_t *pd = VIRT(get_cr3());

    struct page_directory_entry_t *cur_dir = NULL;
    struct page_table_entry_t *cur_table = NULL;
    int k = 0;
    //for (int i = (VIRT_BASE >> 22); i < KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES + (VIRT_BASE >> 22); i++, k++) {
    for (int i = 0; i < 1024; i++, k++) {
        cur_dir = &pd[i];
        if (cur_dir->present) {
            cur_table = (struct page_table_entry_t *)VIRT(cur_dir->page_table_addr << 12);
            printf("[%u] Addr: %x [[%x->%x]->[%x->%x]->[%x->%x]] s:%x rw:%x p:%x\n",
                   i,
                   cur_dir->page_table_addr,
                   // VIRT(cur_dir->page_table_addr << 12),
                   // &kernel_page_table[1024 * k],
                   (i << 22) + (0 << 12),
                   (cur_dir->present ? cur_table[0].page_phys_addr << 12 : 0),
                   (i << 22) + (1 << 12),
                   (cur_dir->present ? cur_table[1].page_phys_addr << 12 : 0),
                   (i << 22) + (1023 << 12),
                   (cur_dir->present ? cur_table[1023].page_phys_addr << 12 : 0),
                   cur_dir->user_supervisor,
                   cur_dir->read_write,
                   cur_dir->present);
        }
    }
    for (int i = 256; i < 256 + 128; i += 4) {
        printf("PB[%u]: %x %x %x %x\n", i, pages_bitmap[i], pages_bitmap[i + 1], pages_bitmap[i + 2], pages_bitmap[i + 3]);
    }
}

struct page_directory_entry_t *create_page_directory() {
    struct page_directory_entry_t *dir;
    dir = kmalloc_a(sizeof(struct page_directory_entry_t) * 1024, 4096);
    memcpy((void *)dir, (void *)kernel_page_directory, sizeof(struct page_directory_entry_t) * 1024);
    return dir;
}

struct page_table_entry_t *create_page_table(size_t count) {
    if (count > 1024) return NULL;
    struct page_table_entry_t *table;
    table = kmalloc_a(sizeof(struct page_table_entry_t) * count, 4096);
    memset((void *)table, 0, sizeof(struct page_table_entry_t) * count);
    return table;
}

void *alloc_page(struct page_table_entry_t *pt, size_t liner_addr) {
    // bind_page(pt, liner_addr, phys_addr);
    for (int i = PHYS_TASKS_SPACE_START; i < PHYS_TASKS_SPACE_END; i += PAGE_SIZE) {
        uint8_t x = (pages_bitmap[i >> (12 + 5)] >> ((i >> 12) & 0b11111)) & 0x1;
        if (x)
            continue;
        bind_page(pt, liner_addr, i);
        return (void *)i;
        // printf("i: %x; a: %u; b: %x; c: %x\n", i, i >> (12 + 5), ((i >> 12) & 0b11111), );
        // break;
    }
    return 0;
}

void bind_addr(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr) {
    bind_table(pd, pt, liner_addr);
    bind_page(pt, liner_addr, phys_addr);
}

void bind_table(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr) {
    if (((size_t)liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END && !kernel_pages_operations))
        return;
    uint32_t PDE = liner_addr >> 22;
    pd[PDE].present = 1;
    pd[PDE].read_write = 1;
    pd[PDE].user_supervisor = 0;
    pd[PDE].page_table_addr = PHYS((size_t)pt) >> 12;
}

void bind_page(struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr) {
    if (((size_t)liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END && !kernel_pages_operations) || ((size_t)phys_addr < PHYS(KERNEL_PAGES_END) && !kernel_pages_operations))
        return;
    uint32_t PTE = (liner_addr & TABLE_BIT_FIELD) >> 12;
    pt[PTE].present = 1;
    pt[PTE].read_write = 1;
    pt[PTE].user_supervisor = 0;
    pt[PTE].page_phys_addr = phys_addr >> 12;
    pages_bitmap[pt[PTE].page_phys_addr >> 5] |= 1 << (pt[PTE].page_phys_addr & 0b11111);  // select page in bitmap
    /* printf("%x (%u) => %x (%u) => %x (%u)\n",
           phys_addr, phys_addr,
           phys_addr >> (12 + 5), phys_addr >> (12 + 5),
           ((phys_addr >> 12) & 0b11111), ((phys_addr >> 12) & 0b11111)); */
}

void unbind_page(struct page_table_entry_t *pt, size_t liner_addr) {
    if ((liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END && !kernel_pages_operations))
        return;
    uint32_t PTE = (liner_addr & TABLE_BIT_FIELD) >> 12;
    // free page in bitmap
    // make 0b0...1...0, invert and AND, which makes 0b1...0...1
    pages_bitmap[pt[PTE].page_phys_addr >> 5] &= ~(1 << (pt[PTE].page_phys_addr & 0b11111));
    pt[PTE].present = 0;
    pt[PTE].page_phys_addr = 0;
}

void unbind_table(struct page_directory_entry_t *pd, size_t liner_addr) {
    if ((liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END && !kernel_pages_operations))
        return;
    uint32_t PDE = liner_addr >> 22;

    pd[PDE].present = 0;
    pd[PDE].read_write = 1;
    pd[PDE].user_supervisor = 0;
    pd[PDE].page_table_addr = 0;
}

void free_page_directory(struct page_directory_entry_t *pd) {
    if (pd != kernel_page_directory)
        kfree_a(pd);
}

void free_page_table(struct page_table_entry_t *pt) {
    if (pt != kernel_page_table)
        kfree_a(pt);
}
