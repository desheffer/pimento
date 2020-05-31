#include <kstdlib.h>
#include <page.h>

/**
 * Free memory in the kernel address space.
 */
void kfree(void * addr)
{
    // @TODO: Free the appropriate amount of memory.
    page_free(addr);
}
