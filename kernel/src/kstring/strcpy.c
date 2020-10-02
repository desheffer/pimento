#include "pimento.h"

char * strcpy(char * dest, const char * src)
{
    char * cdest = dest;
    const char * csrc = src;

    while (1) {
        if (*csrc == '\0') {
            *(cdest++) = '\0';
            break;
        } else {
            *(cdest++) = *(csrc++);
        }
    }

    return dest;
}
