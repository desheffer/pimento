#include <assert.h>
#include <elf.h>
#include <string.h>

void* elf_load(const char* start, size_t length)
{
    assert(length >= sizeof(elf64_hdr_t));

    elf64_hdr_t* header = (elf64_hdr_t*) start;

    // Verify the magic number.
    assert(header->e_ident[EI_MAG0] == 0x7F);
    assert(header->e_ident[EI_MAG1] == 0x45);
    assert(header->e_ident[EI_MAG2] == 0x4C);
    assert(header->e_ident[EI_MAG3] == 0x46);

    // Verify 64-bit format.
    assert(header->e_ident[EI_CLASS] == 0x02);

    // Verify little endiannes.
    assert(header->e_ident[EI_DATA] == 0x01);

    // Verify the ELF version.
    assert(header->e_ident[EI_VERSION] == 0x01);

    // Verify the target ABI.
    assert(header->e_ident[EI_OSABI] == 0x00);

    // Verify the object type.
    assert(header->e_type == 0x02);

    // Verify the target architecture.
    assert(header->e_machine == 0xB7);

    // Verify the ELF version.
    assert(header->e_version == 0x01);

    for (int i = 0; i < header->e_phnum; ++i) {
        assert(length >= header->e_phoff + header->e_phentsize * i);
        elf64_phdr_t* pheader = (elf64_phdr_t*) (start + header->e_phoff + header->e_phentsize * i);

        assert(length >= pheader->p_offset + pheader->p_filesz);
        memcpy((void*) pheader->p_vaddr, start + pheader->p_offset, pheader->p_filesz);
    }

    // @TODO: Process section header table.

    return (void*) header->e_entry;
}
