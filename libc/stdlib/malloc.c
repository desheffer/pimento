#include <stdlib.h>

extern char __end;
static long unsigned _next = &__end;

void* malloc(size_t size)
{
    void* ret = (void*) _next;

    // @TODO: Allocate proper memory.
    _next += size + 0x80;
    _next = _next - (_next % 0x80);

    return ret;
}
