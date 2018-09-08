#include <stdlib.h>

extern char __end;
static char* _next = &__end;

void* malloc(size_t size)
{
    void* ret = (void*) _next;

    // @TODO: Allocate proper memory.
    _next += size + 0x80;
    _next = _next - ((long unsigned) _next % 0x80);

    return ret;
}
