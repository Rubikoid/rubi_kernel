#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/asm_lib.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <types.h>

volatile struct page_directory_entry_t *kernel_page_directory;
volatile struct page_table_entry_t *kernel_page_table;
/*
+------------------------+------------------------+------+----------------------------------------------------+
|          PHYS          |         LINEAR         | SIZE |                      PURPOSE                       |
+------------------------+------------------------+------+----------------------------------------------------+
| 0x00000000->0x000FFFFF | 0xC0000000->0xC00FFFFF | 1MB  | bios, vga, etc                                     |
| 0x00100000->0x00FFFFFF | 0xC0100000->0xC0FFFFFF | 15MB | lower kernel code, kernel heap, kernel everything  |
| 0x01000000->0x01FFFFFF | 0xC1000000->0xC1FFFFFF | 16MB | higher kernel code, kernel heap, kernel everything |
+------------------------+------------------------+------+----------------------------------------------------+
|          PHYS          |         LINEAR         | SIZE |                      PURPOSE                       |
+------------------------+------------------------+------+----------------------------------------------------+
| 0x02000000->0x03FFFFFF | 0x00000000->0x01FFFFFF | 32MB | user applications                                  |
| 0x04000000->0x05FFFFFF | 0x00000000->0x01FFFFFF | 32MB | user applications                                  |
| 0x06000000->0x07FFFFFF | 0x00000000->0x01FFFFFF | 32MB | user applications                                  |
+------------------------+------------------------+------+----------------------------------------------------+
    one page is 4kb
    one table is 4mb
    VIRT_BASE is 3gb
*/

// TODO: check for hardcoded addrs everywhere

void init_memory_manager() {
    kernel_page_directory = (struct page_directory_entry_t *)&boot_page_directory;
    kernel_page_table = (struct page_table_entry_t *)&boot_page_table;

    int k = KERNEL_LOWER_TABLES;
    for (int i = KERNEL_LOWER_TABLES + (VIRT_BASE >> 22); i < KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES + (VIRT_BASE >> 22); i++, k++) {
        memset((void *)&kernel_page_directory[i], 0, sizeof(struct page_directory_entry_t));

        kernel_page_directory[i].present = 1;
        kernel_page_directory[i].read_write = 1;
        kernel_page_directory[i].user_supervisor = 0;
        kernel_page_directory[i].page_table_addr = (PHYS((size_t)&kernel_page_table[1024 * k])) >> 12;  // hack to get addr for K table
        for (int j = 0; j < 1024; j++) {
            memset((void *)&kernel_page_table[1024 * k + j], 0, sizeof(struct page_table_entry_t));

            kernel_page_table[1024 * k + j].present = 1;
            kernel_page_table[1024 * k + j].read_write = 1;
            kernel_page_table[1024 * k + j].page_phys_addr = (k * TABLE_SIZE + j * PAGE_SIZE) >> 12;
        }
    }
}

void mmu_dump() {
    volatile struct page_directory_entry_t *pd = VIRT(get_cr3());

    volatile struct page_directory_entry_t *cur_dir = NULL;
    volatile struct page_table_entry_t *cur_table = NULL;
    int k = 0;
    for (int i = (VIRT_BASE >> 22); i < KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES + (VIRT_BASE >> 22); i++, k++) {
        cur_dir = &pd[i];
        if (1 || cur_dir->present) {
            cur_table = (volatile struct page_table_entry_t *)VIRT(cur_dir->page_table_addr << 12);
            printf("[%u] Addr: %x [%x->%x->%x] s:%x rw:%x p:%x\n",
                   i,
                   cur_dir->page_table_addr,
                   // VIRT(cur_dir->page_table_addr << 12),
                   // &kernel_page_table[1024 * k],
                   (i << 22) + (cur_table[0].page_phys_addr << 10),
                   (i << 22) + (cur_table[1].page_phys_addr << 10),
                   // (i << 22) + (cur_table[2].page_phys_addr << 10),
                   (i << 22) + (cur_table[1023].page_phys_addr << 10),
                   cur_dir->user_supervisor,
                   cur_dir->read_write,
                   cur_dir->present);
        }
    }

    for (int i = 0, k = 0; i < 4; i++, k++) {
        cur_dir = &pd[i];
        if (1 || cur_dir->present) {
            cur_table = (volatile struct page_table_entry_t *)VIRT(cur_dir->page_table_addr << 12);
            printf("[%u] Addr: %x [%x->%x->%x] s:%x rw:%x p:%x\n",
                   i,
                   cur_dir->page_table_addr,
                   // VIRT(cur_dir->page_table_addr << 12),
                   // &kernel_page_table[1024 * k],
                   (i << 22) + (cur_dir->present ? cur_table[0].page_phys_addr << 10 : 0),
                   (i << 22) + (cur_dir->present ? cur_table[1].page_phys_addr << 10 : 0),
                   // (i << 22) + (cur_table[2].page_phys_addr << 10),
                   (i << 22) + (cur_dir->present ? cur_table[1023].page_phys_addr << 10 : 0),
                   cur_dir->user_supervisor,
                   cur_dir->read_write,
                   cur_dir->present);
        }
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

void bind_page_table(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, void* liner_addr, void* phys_addr) {
    if(((size_t)liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END) || (size_t)phys_addr < PHYS(KERNEL_PAGES_END))
      return;
    uint32_t PDE = (size_t)liner_addr >> 22;
    uint32_t PTE = ((size_t)liner_addr >> 12) & TABLE_BIT_FIELD;
    uint32_t PT_base_addr = PHYS((size_t)pt) >> 12;

    printf("%x\n", PT_base_addr);

    pd[PDE].present = 1;
    pd[PDE].read_write = 1;
    pd[PDE].user_supervisor = 0;
    pd[PDE].page_table_addr = PT_base_addr;

    pt[PTE].present = 1;
    pt[PTE].read_write = 1;
    pt[PTE].user_supervisor = 0;
    pt[PTE].page_phys_addr = (size_t)phys_addr >> 12;
}
//C01016A3
void unbind_page_table(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, void* liner_addr) {
  if((size_t)liner_addr < KERNEL_PAGES_END)
      return;

}

void free_page_directory(struct page_directory_entry_t *pd) {
    kfree_a(pd);
}

void free_page_table(struct page_table_entry_t *pt) {
    kfree_a(pt);
}
