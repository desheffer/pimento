#include <memory.h>
#include <stdlib.h>

void* malloc(size_t size)
{
    (void) size;

    // @TODO
    return alloc_page();
}
