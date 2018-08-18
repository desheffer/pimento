#include <kstdio.h>
#include <system.h>

void do_invalid()
{
    kputs("Invalid syscall.\n");
    asm volatile("brk #0");
}
