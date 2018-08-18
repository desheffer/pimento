#include <kstdio.h>
#include <system.h>

void do_invalid(process_regs_t* regs)
{
    (void) regs;

    kputs("Invalid syscall.\n");
    asm volatile("brk #0");
}
