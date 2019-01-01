#include <scheduler.h>
#include <system.h>

void sys_exit_group(int status)
{
    // @TODO
    (void) status;

    process_t* process = scheduler_current();
    scheduler_exit(process);

    scheduler_context_switch();
}
