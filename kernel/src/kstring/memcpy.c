#include <kstring.h>
#include <stddef.h>

void * memcpy(void * dest, const void * src, size_t num)
{
    char * cdest = (char *) dest;
    const char * csrc = (const char *) src;

    while (num--) {
        *(cdest++) = *(csrc++);
    }

    return dest;
}
