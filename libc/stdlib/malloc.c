#include <stdlib.h>

static long unsigned _next = 0x12345678;

void* malloc(size_t size)
{
    void* ret = (void*) _next;

    // @TODO
    _next += size + 0x80;
    _next = _next - (_next % 0x80);

    return ret;
}
