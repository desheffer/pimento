#include <scheduler.h>
#include <system.h>

void do_exit(process_regs_t* regs)
{
    int status = (int) regs->x[0];

    // @TODO: Use status.
    (void) status;

    unsigned pid = scheduler_current_pid();
    process_stop(pid);

    while (1) {
        asm volatile("wfi");
    }
}
