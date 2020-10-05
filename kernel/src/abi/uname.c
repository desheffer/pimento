#include <sys/utsname.h>

#include "abi.h"
#include "mm_context.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

SYSCALL_DEFINE1(uname, struct utsname *, buf)
{
    struct task * task = scheduler_current_task();

    // Create `kbuf` in kernel memory.
    struct utsname * kbuf = kcalloc(sizeof(struct utsname));
    strcpy(kbuf->sysname, "Pimento");
    strcpy(kbuf->nodename, "pi");
    strcpy(kbuf->release, "dev");
    strcpy(kbuf->version, __DATE__ " " __TIME__);
    strcpy(kbuf->machine, "aarch64");

    mm_copy_to_user(task->mm_context, buf, kbuf, sizeof(struct utsname));

    kfree(kbuf);

    return 0;
}
