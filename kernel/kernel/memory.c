#include <assert.h>
#include <memory.h>
#include <mm.h>
#include <string.h>
#include <synchronize.h>

static unsigned _page_count = 0;
static page_t* _pages = 0;
static unsigned _last_index = 0;

static int page_index(void* start)
{
    return ((char*) start - (char*) 0) / PAGE_SIZE;
}

static void* page_start(unsigned index)
{
    return (char*) 0 + index * PAGE_SIZE;
}

void memory_init(void)
{
    // @TODO: Get from hardware.
    void* alloc_end = (void*) 0x3F000000;

    _page_count = ((char*) alloc_end - (char*) 0) / PAGE_SIZE;

    // Create pages without using malloc.
    _pages = (page_t*) &__end;;

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

static void* alloc_page(void)
{
    void* pa = 0;

    enter_critical();

    unsigned index = _last_index;
    do {
        index = (index + 1) % _page_count;
    } while (_pages[index].allocated && index != _last_index);

    if (_pages[index].allocated == 0) {
        _pages[index].allocated = 1;
        pa = page_start(index);
    }

    leave_critical();

    if (pa) {
        memset(phys_to_virt(pa), 0, PAGE_SIZE);
    }

    return pa;
}

void* alloc_kernel_page(void)
{
    void* pa = alloc_page();

    assert(pa != 0);

    return phys_to_virt(pa);
}

void* alloc_user_page(process_t* process)
{
    void* pa = alloc_page();

    assert(pa != 0);

    list_push_back(process->pages, pa);

    return phys_to_virt(pa);
}

static void free_page(void* pa)
{
    unsigned index = page_index(pa);

    enter_critical();

    assert(index < _page_count);
    assert(_pages[index].allocated != 0);

    _pages[index].allocated = 0;

    leave_critical();
}

void free_kernel_page(void* va)
{
    free_page(virt_to_phys(va));
}

void free_user_page(process_t* process, void* va)
{
    (void) process;
    (void) va;

    // @TODO
}
