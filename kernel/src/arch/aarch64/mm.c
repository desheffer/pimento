#include <asm/mm.h>

static va_table_t _l0 __attribute__((aligned (PAGE_SIZE))) = {0};
static va_table_t _l1 __attribute__((aligned (PAGE_SIZE))) = {0};

/**
 * Initialize the bare minimum to relocate the kernel.
 */
void mm_init(void)
{
    // L0 row 0: [0xFFFF000000000000 - 0xFFFF007FFFFFFFFF]
    _l0[0] = (uint64_t) &_l1
        | PT_TABLE
        | PT_AF;

    // L1 row 0: [0xFFFF000000000000 - 0xFFFF00003FFFFFFF]
    for (unsigned i = 0; i < VA_TABLE_LENGTH; ++i) {
        _l1[i] = ((uint64_t) PAGE_SIZE * VA_TABLE_LENGTH * VA_TABLE_LENGTH * i)
            | PT_BLOCK
            | PT_AF
            | PT_XN
            | PT_ATTR(MT_DEVICE_nGnRnE);
    }

    // Set the translation table for user space (temporary).
    asm volatile("msr ttbr0_el1, %0" :: "r" (_l0));

    // Set the translation table for kernel space.
    asm volatile("msr ttbr1_el1, %0" :: "r" (_l0));
}
