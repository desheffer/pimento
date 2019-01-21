#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE0(getppid)
{
    struct process * process = scheduler_current();

    return process->ppid;
}
