#include <kstdio.h>
#include <system.h>

registers_t* do_invalid(registers_t* regs)
{
    kprintf("Invalid syscall (%#x)\n", (unsigned) regs->regs[8]);
    asm volatile("brk #0");

    return regs;
}
