#include <scheduler.h>
#include <synchronize.h>
#include <system.h>

void sys_exit(int status)
{
    // @TODO
    (void) status;

    enter_critical();

    process_t* process = scheduler_current();
    process->state = stopping;

    leave_critical();

    scheduler_context_switch();
}
