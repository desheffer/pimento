#include <kstdlib.h>
#include <memory.h>

void kfree(void * ptr)
{
    // @TODO: Free the appropriate size.
    free_kernel_page(ptr);
}
