#include <stdlib.h>
#include <synchronize.h>

// TODO: Write better implementations of malloc() and free(). These ones are
// only intended to get us up and running.
#include <stdint.h>
extern uint8_t __end;
static uint8_t* _ptr = (uint8_t*) 0x10000000;

void* malloc(size_t size)
{
    enter_critical();

    void* alloc = _ptr;
    _ptr += ((size / 0x80) + 1) * 0x80;

    leave_critical();

    return alloc;
}

void free(void*)
{
    enter_critical();

    leave_critical();
}

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void* operator new(size_t, void* ptr)
{
    return ptr;
}

void operator delete(void* ptr)
{
    free(ptr);
}

void operator delete[](void* ptr)
{
    free(ptr);
}
