#include <kernel/memory/mmu.h>
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
| 0x02000000->0x03FFFFFF | 0x02000000->0x03FFFFFF | 32MB | user applications                                  |
+------------------------+------------------------+------+----------------------------------------------------+
    one page is 4kb
    one table is 4mb
    VIRT_BASE is 3gb
*/

// TODO: check for hardcoded addrs everywhere

void init_memory_manager() {
    kernel_page_directory = (struct page_directory_entry_t *)&boot_page_directory;
    kernel_page_table = (struct page_table_entry_t *)&boot_page_table;

    int k = 4;
    for (int i = 4 + (VIRT_BASE >> 22); i < 8 + (VIRT_BASE >> 22); i++, k++) {
        memset((void *)&kernel_page_directory[i], 0, sizeof(struct page_directory_entry_t));

        kernel_page_directory[i].present = 1;
        kernel_page_directory[i].read_write = 1;
        kernel_page_directory[i].user_supervisor = 0;
        kernel_page_directory[i].page_table_addr = (PHYS((size_t)&kernel_page_table[1024 * k])) >> 12;  // hack to get addr for K table
        for (int j = 0; j < 1024; j++) {
            memset((void *)&kernel_page_table[1024 * k + j], 0, sizeof(struct page_table_entry_t));

            kernel_page_table[1024 * k + j].present = 1;
            kernel_page_table[1024 * k + j].read_write = 1;
            kernel_page_table[1024 * k + j].page_phys_addr = ((4 * 1024 * 1024) * (k) + j * 4096) >> 12;  // 0x01000000 is 16 MB
        }
    }
}

void mmu_dump() {
    volatile struct page_directory_entry_t *cur_dir = NULL;
    volatile struct page_table_entry_t *cur_table = NULL;
    int k = 0;
    for (int i = (VIRT_BASE >> 22); i < 8 + (VIRT_BASE >> 22); i++, k++) {
        cur_dir = &kernel_page_directory[i];
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
}

void *alloc_page() {
    /* uint32_t PDE_index = (last_page_ID) >> 22;
    uint32_t PTE_index = ((last_page_ID & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];
    printf("Allocating new page with PDEi: %u PTEi: %x PDE: %x\n", PDE_index, PTE_index, PDE);
    page_directory_ptr[PDE_index] |= 0x003;
    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = ((last_page_ID - VIRT_BASE) & PDTE_BIT_FIELD) + 0x003;
    last_page_ID += PAGE_SIZE;

    return (void *)(last_page_ID - PAGE_SIZE); */
    return 0;
}

void free_page(void *page_addr_in) {
    if (page_addr_in == NULL)
        return;
    /*size_t page_addr = (size_t)page_addr_in;
    uint32_t PDE_index = (page_addr) >> 22;
    uint32_t PTE_index = ((page_addr & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];

    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = 0;

    if (last_page_ID - page_addr == PAGE_SIZE)  // TODO: make normal memory controller, not that stillborn
        last_page_ID -= PAGE_SIZE; */
    return;
}
