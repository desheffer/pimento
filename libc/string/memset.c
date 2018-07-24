#include <string.h>

void* memset(void* dest, int value, size_t num)
{
    char* d = (char*) dest;

    while (num--) {
        *(d++) = (unsigned char) value;
    }

    return dest;
}
