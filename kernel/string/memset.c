#include <string.h>

void * memset(void * dest, int value, size_t num)
{
    char * cdest = (char *) dest;

    while (num--) {
        *(cdest++) = (unsigned char) value;
    }

    return dest;
}
