#include <stdio.h>

void fwrite(const void* ptr, size_t size, size_t count, unsigned fd)
{
    asm volatile(
        "mov x8, #64\n\t"
        "mov x0, %0\n\t"
        "mov x1, %1\n\t"
        "mov x2, %2\n\t"
        "svc #0"
        :
        : "r" (fd), "r" (ptr), "r" (size * count)
        : "x8", "x0", "x1", "x2"
    );
}
