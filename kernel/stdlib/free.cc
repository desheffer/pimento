#include <memory.h>
#include <stdlib.h>

void free(void* ptr)
{
    // @TODO
    Memory::instance()->freePage(ptr);
}
