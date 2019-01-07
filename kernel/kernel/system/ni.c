#include <kstdio.h>
#include <system.h>

long sys_ni(unsigned nr)
{
    kprintf("Syscall (%#x) not implemented\n", nr);

    return -1;
}
