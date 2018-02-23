#include <string.h>

void* memcpy(void* dest, const void* src, size_t bytes)
{
    volatile uint8_t* d = (uint8_t*) dest;
    const char* s = (const char*) src;

    while (bytes--) {
        *(d++) = *(s++);
    }

    return dest;
}

void* memset(void* dest, uint8_t c, size_t bytes)
{
    volatile uint8_t* d = (uint8_t*) dest;
    while (bytes--) {
        *(d++) = c;
    }

    return dest;
}

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
