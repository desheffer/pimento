#include <abi.h>
#include <pimento.h>

SYSCALL_DEFINE1(invalid, int, nr)
{
    kprintf("Invalid syscall (%#x)\n", nr);

    return -ENOSYS;
}
