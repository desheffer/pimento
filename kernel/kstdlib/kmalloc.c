#include <assert.h>
#include <kstdlib.h>
#include <memory.h>
#include <mm.h>

void* kmalloc(size_t size)
{
    assert(size <= PAGE_SIZE);

    // @TODO: Allocate the appropriate size.
    void* pa = alloc_kernel_page();

    failif(pa == 0);

    return pa;
}
