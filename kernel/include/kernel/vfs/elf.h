#include <types.h>

#ifndef KERNEL_VFS_ELF_H_
#define KERNEL_VFS_ELF_H_

#define ELF_MAGIC 0x464c457f

struct __attribute__((__packed__)) elf_header_ident_t {
	uint32_t ei_magic;
	uint8_t ei_class;
	uint8_t ei_data;
	uint8_t ei_version;
	uint8_t ei_osabi;
	uint8_t ei_abiversion;
	uint8_t ei_pad[7];
};


struct __attribute__((__packed__)) elf_header_t {
    struct elf_header_ident_t e_ident;
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry; 		/* virtual address of entry point */
    uint32_t e_phoff; 		/* program headers table offset */
    uint32_t e_shoff; 		/* program headers sections table offset */
    uint32_t e_flags;
    uint16_t e_ehsize; 		/* file header size */
    uint16_t e_phentsize; 	/* single header size */
    uint16_t e_phnum; 		/* headers count */
    uint16_t e_shentsize; 	/* section header size */
    uint16_t e_shnum; 		/* sections headers count */
    uint16_t e_shstrndx;
};

struct __attribute__((__packed__)) elf_program_header_t {
    uint32_t p_type; 	/* segment type */
    uint32_t p_offset; 	/* segment offset from file begin */
    uint32_t p_vaddr; 	/* target virtual address */
    uint32_t p_paddr; 	/* target physical address */
    uint32_t p_filesz; 	/* segment size in file */
    uint32_t p_memsz; 	/* segment size in memory */
    uint32_t p_flags; 	/* permissions and etc */
    uint32_t p_align; 	/* alignment */
};


extern void elf_exec(struct elf_header_t* header);

#endif