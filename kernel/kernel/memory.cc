#include <assert.h>
#include <memory.h>
#include <mmio.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

Memory* Memory::_instance = 0;

static char _reserved[sizeof(Memory)];

extern void* __heap_start;

Memory::Memory(void* allocStart, void* allocEnd, size_t pageSize)
{
    assert(0 < (long unsigned) allocStart);
    assert((long unsigned) allocStart < (long unsigned) allocEnd + PAGE_SIZE);

    _pageSize = pageSize;
    _allocStart = (void*) ((((long unsigned) allocStart + _pageSize - 1) / _pageSize) * _pageSize);
    _pageCount = ((char*) allocEnd - (char*) _allocStart) / _pageSize;

    // Create pages without using new.
    _pages = (page_t*) _allocStart;

    for (unsigned i = 0; i < _pageCount; ++i) {
        _pages[i].allocated = false;
    }

    char* pagesEnd = (char*) &_pages[_pageCount] - 1;
    reserveRange(_pages, pagesEnd);

    _lastIndex = pageIndex(pagesEnd);
}

Memory::~Memory()
{
}

void Memory::init()
{
    assert(!_instance);

    // @TODO: get mem size from boot loader
    void* allocStart = &__heap_start;
    void* allocEnd = (void*) 0xFFFFFFFF;

    // Create instance using placement new.
    _instance = (Memory*) _reserved;
    new (_instance) Memory(allocStart, allocEnd, PAGE_SIZE);

    // Reserve the location of peripheral memory.
    _instance->reserveRange((void*) peripheral_start, (void*) peripheral_end);
}

void Memory::reserveRange(void* reserveStart, void* reserveEnd)
{
    assert((long unsigned) reserveStart < (long unsigned) reserveEnd + PAGE_SIZE);

    unsigned index = pageIndex(reserveStart);
    unsigned pageEnd = pageIndex(reserveEnd);

    for (; index <= pageEnd; ++index) {
        _pages[index].allocated = true;
    }
}

void* Memory::allocPage()
{
    void* ptr = 0;

    enter_critical();

    unsigned index = _lastIndex;
    do {
        index = (index + 1) % _pageCount;
    } while (_pages[index].allocated && index != _lastIndex);

    if (!_pages[index].allocated) {
        _pages[index].allocated = true;
        ptr = pageStart(index);
    }

    leave_critical();

    if (ptr) {
        memset(ptr, 0, _pageSize);
    }

    return ptr;
}

void Memory::freePage(void* ptr)
{
    unsigned index = pageIndex(ptr);

    enter_critical();

    assert(_pages[index].allocated);

    _pages[index].allocated = false;

    leave_critical();
}

unsigned Memory::pageCount() const
{
    return _pageCount;
}

size_t Memory::pageSize() const
{
    return _pageSize;
}

int Memory::pageIndex(void* start) const
{
    return ((char*) start - (char*) _allocStart) / _pageSize;
}

void* Memory::pageStart(unsigned index) const
{
    return (char*) _allocStart + index * _pageSize;
}
