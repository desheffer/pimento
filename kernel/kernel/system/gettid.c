#include <scheduler.h>
#include <system.h>

long sys_gettid(void)
{
    struct process * process = scheduler_current();

    return process->pid;
}
