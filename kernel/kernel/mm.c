#include <assert.h>
#include <memory.h>
#include <mm.h>
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

static void record_alloc(process_t* process, void* pa, void* va, unsigned flags)
{
    page_t* page = malloc(sizeof(page_t));

    assert(page != 0);

    page->pa = pa;
    page->va = va;
    page->flags = flags;

    list_push_back(process->mm_context->pages, page);
}

void mm_create(process_t* process)
{
    process->mm_context = malloc(sizeof(mm_context_t));
    memset(process->mm_context, 0, sizeof(mm_context_t));

    // Initialize list of allocated pages.
    process->mm_context->pages = list_new();

    va_table_t* l0 = alloc_page();
    record_alloc(process, (void*) l0, 0, 0);

    process->mm_context->pgd = phys_to_pgd(l0, process->pid);
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
        va_table_t* new_tab = alloc_page();
        record_alloc(process, (void*) new_tab, 0, 0);

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
    record_alloc(process, pa, va, PAGE_FLAG_USER);

    va_table_to_virt(tab)[index] = (long unsigned) pa |
        PT_PAGE |
        PT_AF |
        PT_USER |
        PT_ATTR(MT_NORMAL);

    return pa;
}

void mm_map_page(process_t* process, void* va, void* pa)
{
    va_table_t* tab = pgd_to_phys(process->mm_context->pgd);

    for (unsigned level = 0; level < 3; ++level) {
        tab = add_table(process, tab, va, level);
    }

    add_page(process, tab, va, virt_to_phys(pa));
}

void mm_switch_to(process_t* process)
{
    pgd_switch_to(process->mm_context->pgd);
}

void data_abort_handler(void* va)
{
    process_t* process = scheduler_current();

    void* pa = alloc_page();

    mm_map_page(process, va, pa);
}
