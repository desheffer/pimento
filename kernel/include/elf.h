#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint64_t elf64_addr_t;
typedef uint16_t elf64_half_t;
typedef int16_t  elf64_shalf_t;
typedef uint64_t elf64_off_t;
typedef int32_t  elf64_sword_t;
typedef uint32_t elf64_word_t;
typedef uint64_t elf64_xword_t;
typedef int64_t  elf64_sxword_t;

#define EI_MAG0       0x00
#define EI_MAG1       0x01
#define EI_MAG2       0x02
#define EI_MAG3       0x03
#define EI_CLASS      0x04
#define EI_DATA       0x05
#define EI_VERSION    0x06
#define EI_OSABI      0x07
#define EI_ABIVERSION 0x08
#define EI_PAD        0x09
#define EI_NIDENT     0x10

typedef struct elf64_hdr_t {
    unsigned char e_ident[EI_NIDENT]; // Various indentifying information
    elf64_half_t e_type;              // Object type
    elf64_half_t e_machine;           // Target architecture
    elf64_word_t e_version;           // Version
    elf64_addr_t e_entry;             // Entry point virtual address
    elf64_off_t e_phoff;              // Program header table offset
    elf64_off_t e_shoff;              // Section header table offset
    elf64_word_t e_flags;             // Flags
    elf64_half_t e_ehsize;            // Header size
    elf64_half_t e_phentsize;         // Program header table entry size
    elf64_half_t e_phnum;             // Program header table entry count
    elf64_half_t e_shentsize;         // Section header table entry size
    elf64_half_t e_shnum;             // Section header table entry count
    elf64_half_t e_shstrndx;          // Section header table
} elf64_hdr_t;

typedef struct elf64_phdr_t {
    elf64_word_t p_type;    // Segment type
    elf64_word_t p_flags;   // Segment flags
    elf64_off_t p_offset;   // Segment offset
    elf64_addr_t p_vaddr;   // Segment virtual address
    elf64_addr_t p_paddr;   // Segment physical address
    elf64_xword_t p_filesz; // Segment size in the file image
    elf64_xword_t p_memsz;  // Segment size in memory
    elf64_xword_t p_align;  // Segment alignment
} elf64_phdr_t;

typedef struct elf64_shdr_t {
    elf64_word_t sh_name;       // Section name
    elf64_word_t sh_type;       // Section type
    elf64_xword_t sh_flags;     // Section flags
    elf64_addr_t sh_addr;       // Section virtual address
    elf64_off_t sh_offset;      // Section offset in the file image
    elf64_xword_t sh_size;      // Section size in the file image
    elf64_word_t sh_link;       // Section index
    elf64_word_t sh_info;       // Section additional information
    elf64_xword_t sh_addralign; // Section alignment
    elf64_xword_t sh_entsize;   // Section size
} elf64_shdr_t;

void* elf_load(const char*, size_t);
