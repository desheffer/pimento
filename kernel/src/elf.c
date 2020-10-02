#include "binprm.h"
#include "elf.h"
#include "mm_context.h"
#include "pimento.h"
#include "vfs.h"

int elf_load(struct binprm * binprm, struct file * file)
{
    // Load the header.
    struct elf64_hdr * header = kmalloc(sizeof(struct elf64_hdr));
    off_t off = 0;
    size_t num = vfs_read(file, (char *) header, sizeof(struct elf64_hdr), &off);
    if (num < sizeof(struct elf64_hdr)) {
        return -ENOEXEC;
    }

    if (
        // Verify the magic number.
        header->e_ident[EI_MAG0] != 0x7F
        || header->e_ident[EI_MAG1] != 0x45
        || header->e_ident[EI_MAG2] != 0x4C
        || header->e_ident[EI_MAG3] != 0x46

        // Verify 64-bit format.
        || header->e_ident[EI_CLASS] != 0x02

        // Verify little endianness.
        || header->e_ident[EI_DATA] != 0x01

        // Verify the ELF version.
        || header->e_ident[EI_VERSION] != 0x01

        // Verify the target ABI.
        || header->e_ident[EI_OSABI] != 0x00

        // Verify the object type.
        || header->e_type != 0x02

        // Verify the target architecture.
        || header->e_machine != elf_target_arch()

        // Verify the ELF version.
        || header->e_version != 0x01
    ) {
        return -ENOEXEC;
    }

    for (int i = 0; i < header->e_phnum; ++i) {
        // Load the segment header.
        struct elf64_phdr * pheader = kmalloc(sizeof(struct elf64_phdr));
        off = header->e_phoff + header->e_phentsize * i;
        num = vfs_read(file, (char *) pheader, sizeof(struct elf64_phdr), &off);
        if (num < sizeof(struct elf64_phdr)) {
            return -ENOEXEC;
        }

        // Load the segment content.
        off = pheader->p_offset;
        char * dest = (void *) pheader->p_vaddr;
        char * buf = kmalloc(ELF_BUF);
        size_t remaining = pheader->p_filesz;
        while (remaining > 0) {
            size_t count = remaining < ELF_BUF ? remaining : ELF_BUF;
            num = vfs_read(file, (char *) buf, count, &off);
            if (num < count) {
                return -ENOEXEC;
            }

            mm_copy_to_user(binprm->mm_context, dest, buf, num);

            dest += num;
            remaining -= num;
        }

        kfree(pheader);
        kfree(buf);
    }

    // @TODO: Process section header table.

    binprm->entry = (void *) header->e_entry;

    kfree(header);

    return 0;
}
