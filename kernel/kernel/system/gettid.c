#include <scheduler.h>
#include <system.h>

pid_t sys_gettid(void)
{
    process_t* process = scheduler_current();

    return process->pid;
}
