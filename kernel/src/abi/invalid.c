#include <abi.h>
#include <kstdio.h>

SYSCALL_DEFINE1(invalid, int, nr)
{
    kprintf("Invalid syscall (%#x)\n", nr);

    return -1;
}
