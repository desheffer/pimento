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

static void* virt_to_phys(void* ptr)
{
    return (void*) ((long unsigned) ptr & ~VA_START);
}

static void* phys_to_virt(void* ptr)
{
    return (void*) ((long unsigned) ptr | VA_START);
}

void memory_init_kernel_table()
{
    va_table* tables = (va_table*) virt_to_phys(_heap_start);

    // Reserve space for the tables.
    _heap_start = (char*) _heap_start + 4 * sizeof(va_table);

    // Zero out the tables.
    memset(tables, 0, 4 * sizeof(va_table));

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
            flags |= PT_RO | PT_XN;
        } else {
            flags |= PT_XN;
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
        ptr = phys_to_virt(ptr);
        memset(ptr, 0, PAGE_SIZE);
    }

    return ptr;
}

void free_page(void* ptr)
{
    ptr = virt_to_phys(ptr);
    unsigned index = page_index(ptr);

    enter_critical();

    assert(_pages[index].allocated);

    _pages[index].allocated = 0;

    leave_critical();
}
