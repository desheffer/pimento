#include <assert.h>
#include <exec.h>
#include <kstdlib.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE3(execve, const char *, pname, char * const *, argv, char * const *, envp)
{
    process_exec(pname, argv, envp);

    scheduler_context_switch();

    return -1;
}
