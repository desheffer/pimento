#include <heap.h>
#include <stdlib.h>

void* malloc(size_t size)
{
    return Heap::instance()->alloc(size);
}

void free(void* ptr)
{
    return Heap::instance()->free(ptr);
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
