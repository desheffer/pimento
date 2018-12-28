#include <kstdio.h>
#include <system.h>

void sys_invalid(unsigned nr)
{
    kprintf("Invalid syscall (%#x)\n", nr);
    asm volatile("brk #0");
}
