#include <assert.h>
#include <elf.h>
#include <string.h>

void * elf_load(const char * start, size_t length)
{
    failif(length < sizeof(struct elf64_hdr));

    struct elf64_hdr * header = (struct elf64_hdr *) start;

    // Verify the magic number.
    failif(header->e_ident[EI_MAG0] != 0x7F);
    failif(header->e_ident[EI_MAG1] != 0x45);
    failif(header->e_ident[EI_MAG2] != 0x4C);
    failif(header->e_ident[EI_MAG3] != 0x46);

    // Verify 64-bit format.
    failif(header->e_ident[EI_CLASS] != 0x02);

    // Verify little endiannes.
    failif(header->e_ident[EI_DATA] != 0x01);

    // Verify the ELF version.
    failif(header->e_ident[EI_VERSION] != 0x01);

    // Verify the target ABI.
    failif(header->e_ident[EI_OSABI] != 0x00);

    // Verify the object type.
    failif(header->e_type != 0x02);

    // Verify the target architecture.
    failif(header->e_machine != 0xB7);

    // Verify the ELF version.
    failif(header->e_version != 0x01);

    for (int i = 0; i < header->e_phnum; ++i) {
        failif(length < header->e_phoff + header->e_phentsize * i);
        struct elf64_phdr * pheader = (struct elf64_phdr *) (start + header->e_phoff + header->e_phentsize * i);

        failif(length < pheader->p_offset + pheader->p_filesz);
        memcpy((void *) pheader->p_vaddr, start + pheader->p_offset, pheader->p_filesz);
    }

    // @TODO: Process section header table.

    return (void *) header->e_entry;
}
