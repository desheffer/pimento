#include "pimento.h"

size_t strnlen(const char * str, size_t num)
{
    size_t size = 0;

    while (num-- && *(str++)) {
        ++size;
    }

    return size;
}
