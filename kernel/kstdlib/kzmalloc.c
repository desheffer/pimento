#include <kstdlib.h>
#include <string.h>

void * kzalloc(size_t size)
{
    void * pa = kmalloc(size);

    memset(pa, 0, size);

    return pa;
}
