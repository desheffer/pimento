#include <abi.h>
#include <pimento.h>
#include <task.h>

SYSCALL_DEFINE6(clone, int, flags, void *, child_stack, void *, arg, pid_t *, ptid, void *, newtls, pid_t *, ctid)
{
    (void) flags;
    (void) child_stack;
    (void) arg;
    (void) ptid;
    (void) newtls;
    (void) ctid;

    kputs("clone() not implemented.\n");

    return 0;
}
