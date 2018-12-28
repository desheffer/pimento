#include <assert.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>

void mm_init(void)
{
    va_table_t* tables = (va_table_t*) virt_to_phys(&__va_table_start);

    memset(tables, 0, 4 * sizeof(va_table_t));

    // L0 row 0: [0xFFFF000000000000 - 0xFFFF007FFFFFFFFF]
    tables[0][0] = (long unsigned) &tables[1] |
        PT_TABLE |
        PT_AF;

    // L1 row 0: [0xFFFF000000000000 - 0xFFFF00003FFFFFFF]
    tables[1][0] = (long unsigned) &tables[2] |
        PT_TABLE |
        PT_AF;

    // L1 row 1: [0xFFFF000040000000 - 0xFFFF00007FFFFFFF]
    tables[1][1] = (long unsigned) 0x40000000 |
        PT_BLOCK |
        PT_AF |
        PT_XN |
        PT_ATTR(MT_DEVICE_nGnRnE);

    // L2 row 0: [0xFFFF000000000000 - 0xFFFF0000001FFFFF]
    tables[2][0] = (long unsigned) &tables[3] |
        PT_TABLE |
        PT_AF;

    // L2 rows 1 - 511: [0xFFFF000000200000 - 0xFFFF00003FFFFFFF]
    for (long unsigned i = 1; i < VA_TABLE_LENGTH; ++i) {
        long unsigned addr = i * PAGE_SIZE * VA_TABLE_LENGTH;
        long unsigned flags = 0;

        if (addr < 0x3F000000) {
            flags |= PT_ATTR(MT_NORMAL);
        } else {
            flags |= PT_ATTR(MT_DEVICE_nGnRnE);
        }

        tables[2][i] = addr |
            PT_BLOCK |
            flags |
            PT_AF |
            PT_XN;
    }

    // L3 rows 0 - 511: [0xFFFF000000000000 - 0xFFFF0000001FFFFF]
    // This block assumes a 2M limit on the text and rodata sections.
    for (long unsigned i = 0; i < VA_TABLE_LENGTH; ++i) {
        long unsigned addr = i * PAGE_SIZE;
        long unsigned flags = 0;

        if (addr >= (long unsigned) virt_to_phys(&__text_start)
            && addr < (long unsigned) virt_to_phys(&__text_end)
        ) {
            flags |= PT_RO;
        } else if (addr >= (long unsigned) virt_to_phys(&__rodata_start)
            && addr < (long unsigned) virt_to_phys(&__rodata_end)
        ) {
            flags |= PT_RO | PT_PXN;
        } else {
            flags |= PT_PXN;
        }

        tables[3][i] = addr |
            PT_PAGE |
            flags |
            PT_AF |
            PT_ATTR(MT_NORMAL);
    }

    // Set the translation table for user space (temporary).
    asm volatile("msr ttbr0_el1, %0" :: "r" (tables));

    // Set the translation table for kernel space.
    asm volatile("msr ttbr1_el1, %0" :: "r" (tables));
}

void mm_create(process_t* process)
{
    va_table_t* l0 = virt_to_phys(alloc_user_page(process));

    process->ttbr = phys_to_ttbr(l0, process->pid);
}

static unsigned va_table_index(void* va, unsigned level)
{
    assert(level < 4);

    return ((long unsigned) va >> (39 - 9 * level)) & 0x1FF;
}

static va_table_t* add_table(process_t* process, va_table_t* tab, void* va, unsigned level)
{
    unsigned index = va_table_index(va, level);

    if (!(va_table_to_virt(tab)[index] & PT_TABLE)) {
        va_table_t* new_tab = virt_to_phys(alloc_user_page(process));

        va_table_to_virt(tab)[index] = (long unsigned) new_tab |
            PT_TABLE |
            PT_AF;
    }

    long unsigned row = va_table_to_virt(tab)[index];
    return (va_table_t*) (row & VA_TABLE_TABLE_ADDR_MASK);
}

static void* add_page(process_t* process, va_table_t* tab, void* va, void* pa)
{
    unsigned index = va_table_index(va, 3);

    pa = (void*) ((long unsigned) pa & PAGE_MASK);
    list_push_back(process->pages, pa);

    va_table_to_virt(tab)[index] = (long unsigned) pa |
        PT_PAGE |
        PT_AF |
        PT_USER |
        PT_ATTR(MT_NORMAL);

    return pa;
}

void mm_map_page(process_t* process, void* va, void* pa)
{
    va_table_t* tab = ttbr_to_phys(process->ttbr);

    for (unsigned level = 0; level < 3; ++level) {
        tab = add_table(process, tab, va, level);
    }

    add_page(process, tab, va, virt_to_phys(pa));
}

void mm_switch_to(process_t* process)
{
    ttbr_switch_to(process->ttbr);
}

void data_abort_handler(void* va)
{
    process_t* process = scheduler_current();

    void* pa = alloc_user_page(process);

    mm_map_page(process, va, pa);
}
