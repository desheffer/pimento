#include <assert.h>
#include <memory.h>
#include <stdlib.h>

void* malloc(size_t size)
{
    assert(size <= PAGE_SIZE);

    // @TODO: Allocate the appropriate size.
    return alloc_kernel_page();
}
