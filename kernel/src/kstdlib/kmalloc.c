#include "page.h"
#include "pimento.h"

/**
 * Allocate memory in the kernel address space.
 */
void * kmalloc(size_t size)
{
    // @TODO: Allocate the appropriate amount of memory. For now, we can keep
    // it simple and waste an entire page.
    (void) size;

    struct page * page = page_alloc();

    return page->vaddr;
}
