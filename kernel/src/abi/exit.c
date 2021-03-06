#include "abi.h"
#include "exit.h"
#include "pimento.h"
#include "scheduler.h"

SYSCALL_DEFINE1(exit, int, code)
{
    struct task * task = scheduler_current_task();

    exit(task, code);

    scheduler_schedule();

    return 0;
}
