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

registers_t* sys_execve(registers_t* regs)
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

    // Allocate the first page of the interrupt stack.
    void* int_stack_top = (char*) alloc_user_page(process) + PAGE_SIZE;

    // Load child stack with argv and envp.
    void* stack_top = (void*) STACK_TOP;
    stack_top = process_set_args(stack_top, argv, envp);

    // Initialize execution.
    registers_t* new_regs = (registers_t*) int_stack_top - 1;
    new_regs->sp = (long unsigned) stack_top;
    new_regs->pc = (long unsigned) elf_load(&__shell_start, &__shell_end - &__shell_start);
    new_regs->pstate = PSR_MODE_USER;

    leave_critical();

    free(argv);
    free(envp);

    return new_regs;
}
