#include <assert.h>
#include <bcm2837.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

extern char __end;
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
    _heap_start = (char*) _heap_start + 2 * sizeof(va_table);

    // Zero out the tables.
    for (unsigned i = 0; i < 2; ++i) {
        for (unsigned j = 0; j < VA_TABLE_LENGTH; ++j) {
            tables[i][j] = 0;
        }
    }

    tables[0][0] = (long unsigned) &tables[1] |
        PT_TABLE |
        PT_AF;

    // @TODO: Use separate pages for code, data, etc.
    tables[1][0] = (long unsigned) 0x0 |
        PT_BLOCK |
        PT_AF |
        PT_ATTR(MT_DEVICE_nGnRnE);

    tables[1][1] = (long unsigned) 0x40000000 |
        PT_BLOCK |
        PT_AF |
        PT_ATTR(MT_DEVICE_nGnRnE);

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
