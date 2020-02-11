#include <kernel/vfs/elf.h>
#include <kernel/scheduler/task.h>
#include <kernel/memory/mmu.h>
#include <kernel/memory/heap.h>
#include <lib/stdio.h>


void elf_exec(struct elf_header_t* header) {
    assert(header->e_ident.ei_magic == ELF_MAGIC);

    size_t elf_base = (size_t)header;
    size_t entry_point;
    entry_point = header->e_entry;

    struct task_mem_t task_mem = {0};

    task_mem.page_dir = create_page_directory();
    task_mem.page_table = create_page_table(1); // i belive we don't need more than one table ;F
    task_mem.pages_count = 0;

    assert(task_mem.pages == NULL);


    for(int i = 0; i < header->e_phnum; i++) {
        struct elf_program_header_t* p_header = (void*)(header->e_phoff + elf_base + i * header->e_phentsize);
        
        if(p_header->p_memsz == 0) // if section is empty
            continue;
        
        uint32_t pages_count = (p_header->p_memsz / PAGE_SIZE) + 1;

        assert(task_mem.pages_count > 0);
        
        // void* section = (void*)(elf_base + p_header->p_offset);
        
        task_mem.pages = alloc_page(task_mem.page_table, 0);
        for(int i = 0; i < task_mem.pages_count; i++) {

        }
        task_mem.pages_count += pages_count;
        //task_mem.pages = alloc_page(struct page_table_entry_t *pt, size_t liner_addr)(task_mem.pages_count);
    }
}