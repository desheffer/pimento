#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>

SYSCALL_DEFINE1(getpgid, pid_t, pid)
{
    (void) pid;

    return 0;
}
