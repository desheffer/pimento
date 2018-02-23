#include <stdlib.h>

// TODO: Write better implementations of malloc() and free(). These ones are
// only intended to get us up and running.
static char* ptr = (char*) 0x800000;

void* malloc(size_t size)
{
    void* alloc = ptr;
    ptr += ((size / 0x80) + 1) * 0x80;
    return alloc;
}

void free(void*)
{
}

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void* block)
{
    free(block);
}

void operator delete[](void* block)
{
    free(block);
}
