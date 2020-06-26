#pragma once

#include <pimento.h>
#include <task.h>
#include <vfs.h>

#define EI_MAG0       0x00
#define EI_MAG1       0x01
#define EI_MAG2       0x02
#define EI_MAG3       0x03
#define EI_CLASS      0x04
#define EI_DATA       0x05
#define EI_VERSION    0x06
#define EI_OSABI      0x07
#define EI_ABIVERSION 0x08

#define ELF_BUF 1024

struct elf64_hdr {
    uint8_t  e_ident[16]; // Identifier
    uint16_t e_type;      // Object type
    uint16_t e_machine;   // Target architecture
    uint32_t e_version;   // Version
    uint64_t e_entry;     // Entry point virtual address
    uint64_t e_phoff;     // Program header table offset
    uint64_t e_shoff;     // Section header table offset
    uint32_t e_flags;     // Flags
    uint16_t e_ehsize;    // Header size
    uint16_t e_phentsize; // Program header table entry size
    uint16_t e_phnum;     // Program header table entry count
    uint16_t e_shentsize; // Section header table entry size
    uint16_t e_shnum;     // Section header table entry count
    uint16_t e_shstrndx;  // Section header table
};

struct elf64_phdr {
    uint32_t p_type;   // Segment type
    uint32_t p_flags;  // Segment flags
    uint64_t p_offset; // Segment offset
    uint64_t p_vaddr;  // Segment virtual address
    uint64_t p_paddr;  // Segment physical address
    uint64_t p_filesz; // Segment size in the file image
    uint64_t p_memsz;  // Segment size in memory
    uint64_t p_align;  // Segment alignment
};

struct elf64_shdr {
    uint32_t sh_name;      // Section name
    uint32_t sh_type;      // Section type
    uint64_t sh_flags;     // Section flags
    uint64_t sh_addr;      // Section virtual address
    uint64_t sh_offset;    // Section offset in the file image
    uint64_t sh_size;      // Section size in the file image
    uint32_t sh_link;      // Section index
    uint32_t sh_info;      // Section additional information
    uint64_t sh_addralign; // Section alignment
    uint64_t sh_entsize;   // Section size
};

int elf_load(struct binprm *, struct file *);
unsigned elf_target_arch(void);
