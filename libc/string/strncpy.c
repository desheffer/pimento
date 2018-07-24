#include <string.h>

char* strncpy(char* dest, const char* src, size_t num)
{
    char* d = dest;
    const char* s = src;

    while (num--) {
        if (*s == '\0') {
            *(d++) = '\0';
        } else {
            *(d++) = *(s++);
        }
    }

    return dest;
}
