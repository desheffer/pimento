#include <assert.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>

void mmap_create(process_t* process)
{
    va_table_t* l0 = alloc_page();
    push_back(process->pages, l0);

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
        va_table_t* new_tab = alloc_page();
        push_back(process->pages, new_tab);

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
    push_back(process->pages, pa);

    va_table_to_virt(tab)[index] = (long unsigned) pa |
        PT_PAGE |
        PT_AF |
        PT_USER |
        PT_ATTR(MT_NORMAL);

    return pa;
}

void mmap_map_page(process_t* process, void* va, void* pa)
{
    va_table_t* tab = ttbr_to_phys(process->ttbr);

    for (unsigned level = 0; level < 3; ++level) {
        tab = add_table(process, tab, va, level);
    }

    add_page(process, tab, va, pa);
}

void mmap_switch(process_t* process)
{
    switch_ttbr(process->ttbr);
}

void* alloc_user_page(process_t* process, void* va)
{
    void* pa = alloc_page();
    if (pa == 0) {
        return 0;
    }

    mmap_map_page(process, va, pa);

    return phys_to_virt(pa);
}

void data_abort_handler(void* va)
{
    process_t* process = scheduler_current();

    alloc_user_page(process, va);
}
