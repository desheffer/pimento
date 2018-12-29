#include <string.h>

char* strncpy(char* dest, const char* src, size_t num)
{
    char* cdest = dest;
    const char* csrc = src;

    while (num--) {
        if (*csrc == '\0') {
            *(cdest++) = '\0';
        } else {
            *(cdest++) = *(csrc++);
        }
    }

    return dest;
}
