#include <stddef.h>
#include <stdint.h>
#include <string.h>

int32_t strcmp(const char* str1, const char* str2)
{
    while (*str1 && *str1 == *str2) {
        str1++;
        str2++;
    }

    return *str1 < *str2 ? -1 : *str1 > *str2 ? 1 : 0;
}

size_t strlen(const char* str)
{
    size_t size = 0;

    while (*(str++)) {
        size++;
    }

    return size;
}
