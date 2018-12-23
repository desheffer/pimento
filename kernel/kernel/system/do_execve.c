#include <assert.h>
#include <elf.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>
#include <system.h>

extern char __shell_start;
extern char __shell_end;

process_regs_t* do_execve(process_regs_t* regs)
{
    char* pname = (char*) regs->regs[0];
    /* char* const argv[] = (char* const[]) regs->regs[1]; */
    /* char* const envp[] = (char* const[]) regs->regs[2]; */

    // @TODO: Support arbitrary files.
    assert(strcmp("/bin/sh", pname) == 0);

    enter_critical();

    process_t* process = scheduler_current();

    // Update process name.
    strncpy(process->pname, pname, PNAME_LENGTH);

    // Initialize list of allocated pages.
    // @TODO: Reap old pages.
    process->pages = malloc(sizeof(list_t));

    // Initialize a new memory map.
    mmap_create(process);
    mmap_switch(process);

    // Begin a new stack.
    void* stack = (void*) STACK_TOP;

    // @TODO: Set argv and envp.

    // Set startup parameters.
    stack = (void*) ((process_regs_t*) stack - 1);
    regs = (process_regs_t*) stack;
    regs->pstate = PSR_MODE_USER;
    regs->pc = (long unsigned) elf_load(&__shell_start, &__shell_end - &__shell_start);
    process->sp = regs;

    leave_critical();

    return regs;
}
