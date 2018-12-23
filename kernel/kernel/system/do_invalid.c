#include <kstdio.h>
#include <system.h>

process_regs_t* do_invalid(process_regs_t* regs)
{
    kprintf("Invalid syscall (%#x)\n", (unsigned) regs->regs[8]);
    asm volatile("brk #0");

    return regs;
}
