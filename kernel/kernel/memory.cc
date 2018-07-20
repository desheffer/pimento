#include <assert.h>
#include <memory.h>
#include <mmio.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

extern uint8_t __end;

Memory* Memory::_instance = 0;

Memory::Memory(size_t allocSize)
{
    unsigned index;
    unsigned end;

    _allocSize = allocSize;
    _pageCount = _allocSize / PAGE_SIZE;

    // Create pages without using new.
    _pages = (page_t*) (this + 1);

    for (index = 0; index < _pageCount; ++index) {
        _pages[index].allocated = false;
    }

    // Reserve the location of everything allocated so far.
    end = pageIndex(&_pages[_pageCount] - 1);
    for (index = 0; index <= end; ++index) {
        _pages[index].allocated = true;
    }

    // Reserve the location of peripheral memory.
    index = pageIndex((void*) peripheral_start);
    end = pageIndex((void*) peripheral_end);
    for (; index <= end; ++index) {
        _pages[index].allocated = true;
    }

    _firstCandidate = pageIndex(&_pages[_pageCount]);
}

Memory::~Memory()
{
}

void Memory::init()
{
    assert(!_instance);

    // @TODO: get mem size from boot loader
    size_t allocSize = 0xFFFFFFFF - __end;

    // Create instance using placement new.
    _instance = (Memory*) &__end;
    new ((void*) _instance) Memory(allocSize);
}

void* Memory::allocPage()
{
    assert(_pages);

    enter_critical();

    // @TODO: maintain a list of unallocated pages
    unsigned index = _firstCandidate;
    while (_pages[index].allocated) {
        ++index;
    }

    assert(!_pages[index].allocated);

    _pages[index].allocated = true;

    leave_critical();

    void* ptr = pageStart(index);
    memset(ptr, 0, PAGE_SIZE);

    return ptr;
}

void Memory::freePage(void* ptr)
{
    assert(_pages);

    unsigned index = pageIndex(ptr);

    enter_critical();

    assert(_pages[index].allocated);

    _pages[index].allocated = false;

    leave_critical();
}

unsigned Memory::allocSize() const
{
    return _allocSize;
}

unsigned Memory::pageCount() const
{
    return _pageCount;
}

void* Memory::pageStart(unsigned index) const
{
    return ((uint8_t*) 0) + index * PAGE_SIZE;
}

unsigned Memory::pageIndex(void* start) const
{
    return ((size_t) start) / PAGE_SIZE;
}

void* alloc_page()
{
    Memory* memory = Memory::instance();

    assert(memory);

    return memory->allocPage();
}

void free_page(void* ptr)
{
    Memory* memory = Memory::instance();

    assert(memory);

    memory->freePage(ptr);
}
