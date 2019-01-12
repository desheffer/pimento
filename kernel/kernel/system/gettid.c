#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE0(gettid)
{
    struct process * process = scheduler_current();

    return process->pid;
}
