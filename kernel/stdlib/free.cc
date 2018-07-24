#include <heap.h>
#include <stdlib.h>

void free(void* ptr)
{
    return Heap::instance()->free(ptr);
}
