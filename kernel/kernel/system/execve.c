#include <assert.h>
#include <elf.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>
#include <system.h>

int sys_execve(const char* pname, char* const argv[], char* const envp[])
{
    process_exec(pname, argv, envp);

    return -1;
}
