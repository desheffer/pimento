#include <assert.h>
#include <memory.h>
#include <stdlib.h>

void* malloc(size_t size)
{
    assert(size <= PAGE_SIZE);

    // @TODO: Allocate the appropriate size.
    return phys_to_virt(alloc_page());
}
