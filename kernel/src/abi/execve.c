#include <abi.h>
#include <exec.h>
#include <pimento.h>
#include <task.h>

SYSCALL_DEFINE3(execve, const char *, pname, char * const *, argv, char * const *, envp)
{
    // @TODO: Safe copy from user space.
    return exec(pname, argv, envp);
}
