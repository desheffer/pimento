#include <heap.h>
#include <stdlib.h>

void* malloc(size_t size)
{
    return Heap::instance()->alloc(size);
}
