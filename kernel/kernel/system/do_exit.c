#include <synchronize.h>
#include <system.h>

void do_exit(process_regs_t* regs)
{
    int status = (int) regs->regs[0];

    // @TODO: Use status.
    (void) status;

    enter_critical();

    process_t* process = process_current();
    process->state = stopping;

    leave_critical();

    while (1) {
        asm volatile("wfi");
    }
}
