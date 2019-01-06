#include <string.h>

size_t strnlen(const char * str, size_t count)
{
    size_t size = 0;

    while (*(str++) && size < count) {
        ++size;
    }

    return size;
}
