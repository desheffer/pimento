#include "exit.h"
#include "interrupts.h"
#include "pimento.h"
#include "task.h"

/**
 * Exit from inside of a task.
 */
void exit(struct task * task, int code)
{
    interrupts_disable();

    task->state = zombie;
    task->exit_code = code;
}
