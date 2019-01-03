#include <kstdio.h>
#include <system.h>

int sys_ni(unsigned nr)
{
    kprintf("Syscall (%#x) not implemented\n", nr);

    return -1;
}
