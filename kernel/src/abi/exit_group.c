#include <abi.h>
#include <exit.h>
#include <pimento.h>
#include <scheduler.h>

SYSCALL_DEFINE1(exit_group, int, code)
{
    struct task * task = scheduler_current_task();

    exit(task, code);

    schedule();

    return 0;
}
