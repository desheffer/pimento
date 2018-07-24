#include <stdlib.h>
#include <stdint.h>

static uint64_t _next = 0x12345678;

void* malloc(size_t size)
{
    void* ret = (void*) _next;

    // @TODO
    _next += size;
    _next = _next + (_next % 0x80);

    return ret;
}
