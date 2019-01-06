#include <scheduler.h>
#include <system.h>

pid_t sys_gettid(void)
{
    struct process * process = scheduler_current();

    return process->pid;
}
