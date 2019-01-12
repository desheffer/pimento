#include <process.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE6(clone, int, flags, void *, child_stack, void *, arg, pid_t *, ptid, void *, newtls, pid_t *, ctid)
{
    // @TODO
    (void) flags;
    (void) child_stack;
    (void) arg;
    (void) ptid;
    (void) newtls;
    (void) ctid;

    long cpid = process_clone(scheduler_current());

    return cpid;
}
