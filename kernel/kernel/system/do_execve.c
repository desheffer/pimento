#include <assert.h>
#include <elf.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>
#include <system.h>

extern char __shell_start;
extern char __shell_end;

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

// @TODO: This method is very broken right now.
registers_t* do_execve(registers_t* regs)
{
    char* pname = (char*) regs->regs[0];
    char** argv = copy_args((char* const*) regs->regs[1]);
    char** envp = copy_args((char* const*) regs->regs[2]);

    // @TODO: Support arbitrary files.
    assert(strcmp("/bin/sh", pname) == 0);

    enter_critical();

    process_t* process = scheduler_current();

    // Update process name.
    strncpy(process->pname, pname, PNAME_LENGTH);

    // Initialize list of allocated pages.
    // @TODO: Reap old pages and delete old list.
    process->pages = list_new();

    // Initialize a new memory map.
    mmap_create(process);
    mmap_switch_to(process);

    // Begin a new stack.
    void* stack = (void*) STACK_TOP;

    // Load stack with argv and envp.
    stack = process_set_args(stack, argv, envp);

    // Load stack with initial state.
    stack = (void*) ((registers_t*) stack - 1);
    regs = (registers_t*) stack;
    regs->pstate = PSR_MODE_USER;
    regs->pc = (long unsigned) elf_load(&__shell_start, &__shell_end - &__shell_start);

    leave_critical();

    free(argv);
    free(envp);

    return regs;
}
