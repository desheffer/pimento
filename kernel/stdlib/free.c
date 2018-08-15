#include <memory.h>
#include <stdlib.h>

void free(void* ptr)
{
    // @TODO
    free_page(ptr);
}
