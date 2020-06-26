#include <elf.h>

/**
 * Return the ELF code for AArch64.
 */
unsigned elf_target_arch(void)
{
    return 0xB7;
}
