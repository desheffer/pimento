#include <unistd.h>

void exit(int status)
{
    asm volatile(
        "mov x8, %0\n\t"
        "mov x0, %1\n\t"
        "svc #0"
        :
        : "r" (SYS_EXIT), "r" (status)
        : "x8", "x0"
    );
}
