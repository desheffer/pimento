#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE1(exit_group, int, status)
{
    // @TODO
    (void) status;

    struct process * process = scheduler_current();
    scheduler_exit(process);

    scheduler_context_switch();

    return -1;
}
