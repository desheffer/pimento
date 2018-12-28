#include <assert.h>
#include <elf.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>
#include <system.h>

static char** copy_args(char* const src[])
{
    // Find size of src.
    unsigned src_size = 0;
    for (char* const* iter = src; *iter != 0; ++iter) {
        ++src_size;
    }

    char** dest = malloc(src_size);

    // Copy src to dest.
    for (unsigned i = 0; i < src_size; ++i) {
        dest[i] = malloc(strlen(src[i]) + 1);
        strcpy(dest[i], src[i]);
    }

    return dest;
}

registers_t* sys_execve(registers_t* regs)
{
    char* pname = (char*) regs->regs[0];
    char** argv = copy_args((char* const*) regs->regs[1]);
    char** envp = copy_args((char* const*) regs->regs[2]);

    process_exec(pname, argv, envp);

    return regs;
}
