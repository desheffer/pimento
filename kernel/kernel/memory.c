#include <assert.h>
#include <bcm2837.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

static void* _heap_start = &__end;

static unsigned _page_count = 0;
static page_t* _pages = 0;
static unsigned _last_index = 0;

void memory_init_kernel()
{
    va_table_t* tables = (va_table_t*) virt_to_phys(_heap_start);

    // Reserve space for the tables.
    _heap_start = (char*) _heap_start + 4 * sizeof(va_table_t);

    // Zero out the tables.
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

static int page_index(void* start)
{
    return ((char*) start - (char*) 0) / PAGE_SIZE;
}

static void* page_start(unsigned index)
{
    return (char*) 0 + index * PAGE_SIZE;
}

void memory_init()
{
    // @TODO: Get from hardware.
    void* alloc_end = (void*) 0x3F000000;

    _page_count = ((char*) alloc_end - (char*) 0) / PAGE_SIZE;

    // Create pages without using malloc.
    _pages = (page_t*) _heap_start;

    for (unsigned i = 0; i < _page_count; ++i) {
        _pages[i].allocated = 0;
    }

    void* pages_end = virt_to_phys((char*) &_pages[_page_count] - 1);
    memory_reserve_range(0, pages_end);

    _last_index = page_index(pages_end);
}

void memory_create_process(process_t* process, page_t* init_stack)
{
    // @TODO: Allocate a base table only and allocate more memory as requested.
    va_table_t* l0 = alloc_page();
    push_back(process->pages, l0);
    process->ttbr = phys_to_ttbr(l0, process->pid);

    va_table_t* l1 = alloc_page();
    push_back(process->pages, l1);

    va_table_t* l2 = alloc_page();
    push_back(process->pages, l2);

    va_table_t* l3_f = alloc_page();
    push_back(process->pages, l3_f);

    va_table_t* l3_b = alloc_page();
    push_back(process->pages, l3_b);

    // L0 row 0: [0x0000000000000000 - 0x0000007FFFFFFFFF]
    (*((va_table_t*) phys_to_virt(l0)))[0] = (long unsigned) l1 |
        PT_TABLE |
        PT_AF;

    // L1 row 0: [0x0000000000000000 - 0x000000003FFFFFFF]
    (*((va_table_t*) phys_to_virt(l1)))[0] = (long unsigned) l2 |
        PT_TABLE |
        PT_AF;

    // L2 row 0: [0x0000000000000000 - 0x00000000001FFFFF]
    (*((va_table_t*) phys_to_virt(l2)))[0] = (long unsigned) l3_f |
        PT_TABLE |
        PT_AF;

    // L2 row 511: [0x000000003FE00000 - 0x000000003FFFFFFF]
    (*((va_table_t*) phys_to_virt(l2)))[511] = (long unsigned) l3_b |
        PT_TABLE |
        PT_AF;

    // L3 front rows 0 - 7: [0x0000000000000000 - 0x0000000000008FFF]
    // @TODO: 16 is a made up, magic number.
    for (long unsigned i = 0; i < 16; ++i) {
        void* page = alloc_page();
        push_back(process->pages, page);

        // @TODO: XN, RO, etc...
        long unsigned flags = 0;

        (*((va_table_t*) phys_to_virt(l3_f)))[i] = (long unsigned) page |
            PT_PAGE |
            flags |
            PT_AF |
            PT_USER |
            PT_ATTR(MT_NORMAL);
    }

    // L3 back row 511: [0x000000003FFFEFFF - 0x000000003FFFFFFF]
    (*((va_table_t*) phys_to_virt(l3_b)))[511] = (long unsigned) init_stack |
        PT_PAGE |
        PT_AF |
        PT_USER |
        PT_XN |
        PT_ATTR(MT_NORMAL);
}

void memory_destroy_process(process_t* process)
{
    // @TODO: Call free_page() for each entry in process->pages.
    (void) process;
}

void memory_switch_mm(process_t* process)
{
    asm volatile(
        "msr ttbr0_el1, %0\n\t"
        "ic iallu\n\t"
        "dsb nsh\n\t"
        "isb"
        :
        : "r" (process->ttbr)
    );
}

void memory_reserve_range(void* start, void* end)
{
    unsigned index = page_index(start);
    unsigned page_end = page_index(end);

    while (index <= page_end && index < _page_count) {
        _pages[index++].allocated = 1;
    }
}

void* alloc_page()
{
    void* ptr = 0;

    enter_critical();

    unsigned index = _last_index;
    do {
        index = (index + 1) % _page_count;
    } while (_pages[index].allocated && index != _last_index);

    if (!_pages[index].allocated) {
        _pages[index].allocated = 1;
        ptr = page_start(index);
    }

    leave_critical();

    if (ptr) {
        memset(phys_to_virt(ptr), 0, PAGE_SIZE);
    }

    return ptr;
}

void free_page(void* ptr)
{
    unsigned index = page_index(ptr);

    enter_critical();

    assert(index < _page_count);
    assert(_pages[index].allocated);

    _pages[index].allocated = 0;

    leave_critical();
}
