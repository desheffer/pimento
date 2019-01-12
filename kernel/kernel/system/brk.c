#include <mm.h>
#include <scheduler.h>
#include <system.h>

long sys_brk(void * addr)
{
    struct process * process = scheduler_current();

    if (addr > process->mm_context->brk) {
        process->mm_context->brk = addr;
    }

    return (long) process->mm_context->brk;
}
