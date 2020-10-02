#include "pimento.h"

void * kcalloc(size_t size)
{
    void * pa = kmalloc(size);

    memset(pa, 0, size);

    return pa;
}
