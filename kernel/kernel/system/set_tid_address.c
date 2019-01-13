#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE1(set_tid_address, int *, tidptr)
{
    struct process * process = scheduler_current();

    *tidptr = process->pid;

    return process->pid;
}
