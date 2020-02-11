#include <kernel/asm_lib.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/scheduler/task.h>
#include <kernel/vfs/elf.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

void starter() {
    printf("wtf_starter_func\n");
    syscall_exit();
}

struct task_t* elf_exec(struct elf_header_t* header) {
    assert(header->e_ident.ei_magic == ELF_MAGIC);

    size_t elf_base = (size_t)header;
    size_t entry_point = header->e_entry;

    printf("ELF: entry: %x; phnum: %x\n", header->e_entry, header->e_phnum);

    struct task_mem_t task_mem = {0};

    task_mem.page_dir = create_page_directory();
    task_mem.page_table = create_page_table(16);           // i belive we don't need more than one table ;F
    task_mem.pages = (void**)kmalloc(16 * sizeof(void*));  //FIXME: crazy solution
    task_mem.pages_count = 0;

    bind_table(task_mem.page_dir, task_mem.page_table, 0);

    assert(task_mem.pages != NULL);

    for (int i = 0; i < header->e_phnum; i++) {
        struct elf_program_header_t* p_header = (void*)(header->e_phoff + elf_base + i * header->e_phentsize);
        printf("ELFH: %x (%x)\n", p_header->p_vaddr, p_header->p_memsz);
        if (p_header->p_memsz == 0)  // if section is empty
            continue;

        uint32_t pages_count = (p_header->p_memsz / PAGE_SIZE) + 1;
        void* section = (void*)(elf_base + p_header->p_offset);
        void* start = 0;

        for (int i = 0; i < pages_count; i++) {
            printf("XX:%x\n", p_header->p_vaddr + i * PAGE_SIZE);
            task_mem.pages[task_mem.pages_count] = (void*)p_header->p_vaddr + i * PAGE_SIZE;
            alloc_page(task_mem.page_table, p_header->p_vaddr + i * PAGE_SIZE);
            if (i == 0)
                start = task_mem.pages[task_mem.pages_count];
            task_mem.pages_count += 1;
        }

        /*
            Нельзя просто взять и сделать memcpy, поскольку это происходит В область, которая не замапплена в ядерном пространстве.
            Попытка маппинга этой области в ядерное пространство может привести к взлому жопы, и это один из нескольких выходов, которые я вижу.

            Другой выход - это внаглую прыгнуть в пространство пользователя, но не полностью. 
            Поскольку ядро ВСЕГДА отображается в 3й гигабайт пространства пользователя, то можно включить paging на новом, свежесоздаваемом task_mem'e.
        */
        ;

        void* save_pointer = get_cr3();                         // утащим текущую PD, вдруг она не чисто ядерная.
        enable_paging((void*)PHYS((size_t)task_mem.page_dir));  // переключимся на PD нового таска
        memcpy(start, section, p_header->p_filesz);             // копируем нахой
        if (p_header->p_filesz == 0)                            // handle .bss
            memset(start, 0, p_header->p_memsz);                // fill bss with zeros, because GCC is crazy and code "int x=0;" puts into .bss
        enable_paging(save_pointer);                            // и обратно в ядро

        // task_mem.pages[i] = alloc_page(task_mem.page_table, 0);
        //task_mem.pages_count += pages_count;
        //task_mem.pages = alloc_page(struct page_table_entry_t *pt, size_t liner_addr)(task_mem.pages_count);
    }

    struct task_t* task = task_create(0, (void*)entry_point, &task_mem, "task");
    task->status = TASK_RUNNING;
    return task;
}
