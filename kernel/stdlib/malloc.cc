#include <memory.h>
#include <stdlib.h>

void* malloc(size_t /*size*/)
{
    // @TODO
    return Memory::instance()->allocPage();
}
