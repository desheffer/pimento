#include <assert.h>
#include <heap.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

Heap* Heap::_instance = 0;

Heap::Heap(Memory* memory)
{
    assert(memory);

    _memory = memory;
}

Heap::~Heap()
{
}

void Heap::init(Memory* memory)
{
    assert(!_instance);

    // Create instance using placement new.
    _instance = (Heap*) alloc_page();
    new ((void*) _instance) Heap(memory);
}

Heap* Heap::instance() {
    assert(_instance);

    return _instance;
}

void* Heap::alloc(size_t /*size*/)
{
    enter_critical();

    void* ptr = _memory->allocPage(); // @TODO lol

    leave_critical();

    return ptr;
}

void Heap::free(void* ptr)
{
    enter_critical();

    _memory->freePage(ptr); // @TODO lol

    leave_critical();
}
