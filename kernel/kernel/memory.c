#include <assert.h>
#include <bcm2837.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

static void* _alloc_start = 0;
static unsigned _page_count = 0;
static page_t* _pages = 0;
static unsigned _last_index = 0;

static int memory_page_index(void* start)
{
    return ((char*) start - (char*) _alloc_start) / PAGE_SIZE;
}

static void* memory_page_start(unsigned index)
{
    return (char*) _alloc_start + index * PAGE_SIZE;
}

void memory_init()
{
    _alloc_start = &__heap_start;

    // @TODO: Get from hardware.
    void* _alloc_end = (void*) 0x40000000;

    _page_count = ((char*) _alloc_end - (char*) _alloc_start) / PAGE_SIZE;

    // Round up to the start of the next full page.
    _alloc_start = (void*) ((((long unsigned) _alloc_start + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE);

    // Create pages without using malloc.
    _pages = (page_t*) _alloc_start;

    for (unsigned i = 0; i < _page_count; ++i) {
        _pages[i].allocated = 0;
    }

    char* pages_end = (char*) &_pages[_page_count] - 1;
    memory_reserve_range(_pages, pages_end);

    // Reserve the location of peripheral memory.
    memory_reserve_range((void*) peripheral_start, (void*) peripheral_end);

    _last_index = memory_page_index(pages_end);
}

unsigned memory_page_count()
{
    return _page_count;
}

size_t memory_page_size()
{
    return PAGE_SIZE;
}

void memory_reserve_range(void* start, void* end)
{
    unsigned index = memory_page_index(start);
    unsigned page_end = memory_page_index(end);

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
        ptr = memory_page_start(index);
    }

    leave_critical();

    if (ptr) {
        memset(ptr, 0, PAGE_SIZE);
    }

    return ptr;
}

void free_page(void* ptr)
{
    unsigned index = memory_page_index(ptr);

    enter_critical();

    assert(_pages[index].allocated);

    _pages[index].allocated = 0;

    leave_critical();
}
