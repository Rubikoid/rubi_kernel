#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <types.h>

#define __MODULE_NAME__ "MMU"

struct page_directory_entry_t *kernel_page_directory;
struct page_table_entry_t *kernel_page_table;
//struct page_table_entry_t *kernel_extended_page_table[KERNEL_EXTENDED_TABLES];

uint32_t extended_kernel_pages_bitmap[MM_BITMAP_INT_PER_TABLE * KERNEL_EXTENDED_TABLES];
uint32_t pages_bitmap[MM_BITMAP_INT_PER_TABLE * MM_BITMAP_TABLES];

// boolean for locking operations over kernel pages. i belive this can help don't shoot leg.
uint8_t kernel_pages_operations = 0;
/*
+------------------------+------------------------+-------+----------------------------------------------------+
|          PHYS          |         LINEAR         | SIZE  |                      PURPOSE                       |
+------------------------+------------------------+-------+----------------------------------------------------+
| 0x00000000->0x000FFFFF | 0xC0000000->0xC00FFFFF | 1MB   | bios, vga, etc                                     |
| 0x00100000->0x00FFFFFF | 0xC0100000->0xC0FFFFFF | 15MB  | lower kernel code, kernel stack, kernel everything |
| 0x01000000->0x01FFFFFF | 0xC1000000->0xC1FFFFFF | 16MB  | kernel heap                                        |
| 0xXXXXXXXX->0xXXXXXXXX | 0xC2000000->0xE1FFFFFF | 512MB | Extended kernel space                              |
+------------------------+------------------------+-------+----------------------------------------------------+
|          PHYS          |         LINEAR         | SIZE  |                      PURPOSE                       |
+------------------------+------------------------+-------+----------------------------------------------------+
| 0x02000000->0x03FFFFFF | 0x00000000->0x01FFFFFF | 32MB  | user applications                                  |
| 0x04000000->0x05FFFFFF | 0x00000000->0x01FFFFFF | 32MB  | user applications                                  |
| 0x06000000->0x07FFFFFF | 0x00000000->0x01FFFFFF | 32MB  | user applications                                  |
+------------------------+------------------------+-------+----------------------------------------------------+
    one page is 4kb
    one table is 4mb
    so one table contains 1024 pages;
    VIRT_BASE is 3gb
*/

// TODO: check for hardcoded addrs everywhere

void init_memory_manager() {
    kernel_page_directory = (struct page_directory_entry_t *)&boot_page_directory;
    kernel_page_table = (struct page_table_entry_t *)&boot_page_table;
    memset(pages_bitmap, 0, MM_BITMAP_INT_PER_TABLE * MM_BITMAP_TABLES);
    memset(extended_kernel_pages_bitmap, 0, MM_BITMAP_INT_PER_TABLE * KERNEL_EXTENDED_TABLES);

    /*  first 16 mb of kernel 
        so, they are yet mapped by start.asm, but! they needed to be checked in bitmap.
    */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < MM_BITMAP_INT_PER_TABLE; j++) {
            pages_bitmap[i * MM_BITMAP_INT_PER_TABLE + j] = 0b11111111111111111111111111111111;
        }
    }

    /* second 16 mb of kernel, mapping kernel heap */
    int k = KERNEL_LOWER_TABLES * PAGES_PER_TABLE;
    kernel_pages_operations = 1;
    /* i = from 0xC1000000 to 0xC1FFFFFF -> this is a kernel heap */
    for (size_t i = VIRT_BASE + KERNEL_LOWER_TABLES * TABLE_SIZE; i < VIRT_BASE + (KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE; i += PAGE_SIZE, k += 1) {
        if (i % TABLE_SIZE == 0) {
            bind_table(kernel_page_directory, kernel_page_table + k, i);
        }
        bind_page(kernel_page_table + (k & (TABLE_BIT_FIELD >> 2)), i, PHYS(i), simple_page_bitmap_callback);
    }
    kernel_pages_operations = 0;

    /*for (int i = 0; i < KERNEL_EXTENDED_TABLES; i++) {
        kernel_extended_page_table[i] = create_page_table(1024);
        if (!kernel_extended_page_table[i]) {
            kpanic("Error allocationg kernel extended page tables, at i=%d", i);
        }
    }*/
}

void mmu_dump(struct page_directory_entry_t *pd) {
    //struct page_directory_entry_t *pd = VIRT(get_cr3());

    struct page_directory_entry_t *cur_dir = NULL;
    struct page_table_entry_t *cur_table = NULL;
    int k = 0;
    //for (int i = (VIRT_BASE >> 22); i < KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES + (VIRT_BASE >> 22); i++, k++) {
    for (int i = 0; i < PAGES_PER_TABLE; i++, k++) {
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
    if (count > PAGES_PER_TABLE) return NULL;
    struct page_table_entry_t *table;
    table = kmalloc_a(sizeof(struct page_table_entry_t) * count, 4096);
    memset((void *)table, 0, sizeof(struct page_table_entry_t) * count);
    return table;
}

uint8_t get_page_from_bitmap(uint32_t *bitmap, size_t addr) {
    return (bitmap[addr >> (12 + 5)] >> ((addr >> 12) & 0b11111)) & 0x1;
}

void set_page_to_bitmap(uint32_t *bitmap, size_t addr, uint8_t value) {
    // free page in bitmap
    // make 0b0...1...0, invert and AND, which makes 0b1...0...1
    if (value == 0)  // unset
        pages_bitmap[addr >> (12 + 5)] &= ~(1 << ((addr >> 12) & 0b11111));
    else  // set
        pages_bitmap[addr >> (12 + 5)] |= 1 << ((addr >> 12) & 0b11111);
    // this method has very strange if and implementation, so
    // TODO: think about making this part less magic.
}

void *alloc_page(struct page_table_entry_t *pt, size_t liner_addr) {
    // bind_page(pt, liner_addr, phys_addr);
    for (size_t i = PHYS_TASKS_SPACE_START; i < PHYS_TASKS_SPACE_END; i += PAGE_SIZE) {
        uint8_t x = get_page_from_bitmap(pages_bitmap, i);
        if (x)
            continue;
        bind_page(pt, liner_addr, i, simple_page_bitmap_callback);
        return (void *)i;
        // printf("i: %x; a: %u; b: %x; c: %x\n", i, i >> (12 + 5), ((i >> 12) & 0b11111), );
        // break;
    }
    return 0;
}

void *alloc_page_extended(struct page_table_entry_t *pt, size_t phys_addr) {
    // bind_page(pt, liner_addr, phys_addr);
    for (int i = KERNEL_EXTENDED_START; i < KERNEL_EXTENDED_END; i += PAGE_SIZE) {
        uint8_t x = get_page_from_bitmap(extended_kernel_pages_bitmap, i - KERNEL_EXTENDED_START);
        if (x)
            continue;
        bind_page(pt, i, phys_addr, extended_kernel_bitmap_callback);
        return (void *)i;
        // printf("i: %x; a: %u; b: %x; c: %x\n", i, i >> (12 + 5), ((i >> 12) & 0b11111), );
        // break;
    }
    return 0;
}

void bind_addr(struct page_directory_entry_t *pd, struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr, bitmap_callback_t bm_callb) {
    bind_table(pd, pt, liner_addr);
    bind_page(pt, liner_addr, phys_addr, bm_callb);
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

void bind_page(struct page_table_entry_t *pt, size_t liner_addr, size_t phys_addr, bitmap_callback_t bm_callb) {
    if (((size_t)liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END && !kernel_pages_operations) || ((size_t)phys_addr < PHYS(KERNEL_PAGES_END) && !kernel_pages_operations))
        return;
    uint32_t PTE = (liner_addr & TABLE_BIT_FIELD) >> 12;
    pt[PTE].present = 1;
    pt[PTE].read_write = 1;
    pt[PTE].user_supervisor = 0;
    pt[PTE].page_phys_addr = phys_addr >> 12;

    bm_callb(phys_addr, liner_addr, 1);  // select page in bitmap
    /* printf("%x (%u) => %x (%u) => %x (%u)\n",
           phys_addr, phys_addr,
           phys_addr >> (12 + 5), phys_addr >> (12 + 5),
           ((phys_addr >> 12) & 0b11111), ((phys_addr >> 12) & 0b11111)); */
}

void unbind_page(struct page_table_entry_t *pt, size_t liner_addr, bitmap_callback_t bm_callb) {
    if ((liner_addr > VIRT_BASE && (size_t)liner_addr < KERNEL_PAGES_END && !kernel_pages_operations))
        return;
    uint32_t PTE = (liner_addr & TABLE_BIT_FIELD) >> 12;

    bm_callb(pt[PTE].page_phys_addr << 12, liner_addr, 0);
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

void simple_page_bitmap_callback(uint32_t phys_addr, uint32_t linear_addr, uint32_t set) {
    // between two chairs, where the first is random memory write (due to overflow in bitmap array) and the second is untracked physical page alloc, i select the second
    // SO, this is an exceptional case, which should newer happend, BUT...
    if ((phys_addr >> (12 + 5)) < MM_BITMAP_INT_PER_TABLE * MM_BITMAP_TABLES) {
        set_page_to_bitmap(pages_bitmap, phys_addr, set);  // select page in bitmap
    } else
        klog(
            "Failed to fit page in main bitmap;"
            "PhysAddr=%x; "
            "LinAddr=%x; "
            "set=%x\n",
            phys_addr,
            linear_addr,
            set);
}

void extended_kernel_bitmap_callback(uint32_t phys_addr, uint32_t linear_addr, uint32_t set) {
    if ((linear_addr - KERNEL_EXTENDED_START) >> (12 + 5) < MM_BITMAP_INT_PER_TABLE * KERNEL_EXTENDED_TABLES) {
        set_page_to_bitmap(extended_kernel_pages_bitmap, (linear_addr - KERNEL_EXTENDED_START), set);
        klog(
            "Successfull fit page in extended bitmap;"
            "PhysAddr=%x; "
            "PhysAddr+=%x; "
            "LinAddr=%x; "
            "LinAddr-=%x; "
            "set=%x\n",
            phys_addr,
            phys_addr & PDTE_BIT_FIELD,
            linear_addr,
            linear_addr - KERNEL_EXTENDED_START,
            set);
    } else
        klog(
            "Failed to fit page in extended bitmap;"
            "PhysAddr=%x; "
            "LinAddr=%x; "
            "set=%x\n",
            phys_addr,
            linear_addr,
            set);
}

void free_page_directory(struct page_directory_entry_t *pd) {
    if (pd != kernel_page_directory)  // save from unmapping kernel PD and shooting to leg
        kfree_a(pd);
}

void free_page_table(struct page_table_entry_t *pt) {
    // save from unmapping kernel PT's and shooting to leg
    if (!(pt >= kernel_page_table && pt <= (kernel_page_table + (KERNEL_LOWER_TABLES + KERNEL_HIGHER_TABLES) * TABLE_SIZE)))  // pt != kernel_page_table
        kfree_a(pt);
}
