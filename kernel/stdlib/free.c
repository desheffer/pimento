#include <memory.h>
#include <stdlib.h>

void free(void* ptr)
{
    // @TODO: Free the appropriate size.
    free_page(virt_to_phys(ptr));
}
