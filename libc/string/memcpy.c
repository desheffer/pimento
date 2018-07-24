#include <string.h>

void* memcpy(void* dest, const void* src, size_t num)
{
    char* d = (char*) dest;
    const char* s = (const char*) src;

    while (num--) {
        *(d++) = *(s++);
    }

    return dest;
}
