#include <kstdio.h>
#include <system.h>

void do_invalid(process_regs_t* regs)
{
    kprintf("Invalid syscall (%#x)\n", (unsigned) regs->regs[8]);
    asm volatile("brk #0");
}
