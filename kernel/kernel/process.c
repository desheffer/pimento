#include <assert.h>
#include <elf.h>
#include <list.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

extern char __shell_start;
extern char __shell_end;

process_t* process_create_kernel(void)
{
    // Create a new process control block.
    process_t* process = malloc(sizeof(process_t));
    memset(process, 0, sizeof(process_t));

    // Assign a pid and basic information.
    process->pid = scheduler_assign_pid();
    process->state = running;
    strncpy(process->pname, "kernel", PNAME_LENGTH);

    // Initialize list of allocated pages.
    process->pages = list_new();

    // Initialize memory map.
    mm_create(process);

    // Initialize execution.
    process->cpu_context = malloc(sizeof(cpu_context_t));

    return process;
}

void process_destroy(process_t* process)
{
    (void) process;

    // @TODO: Call free_user_page() for each entry in process->pages.
    // @TODO: Free process.
}

int process_exec(const char* pname, char* const argv[], char* const envp[])
{
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
    mm_create(process);
    mm_switch_to(process);

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

    /* free(argv); */
    /* free(envp); */

    // @TODO: Need to reset kernel stack so we don't overflow.
    do_exec(new_regs);

    return -1;
}

void* process_set_args(void* sp, char* const argv[], char* const envp[])
{
    // Find size of argv.
    unsigned argv_size = 0;
    for (char* const* iter = argv; *iter != 0; ++iter) {
        ++argv_size;
    }

    // Find size of envp.
    unsigned envp_size = 0;
    for (char* const* iter = envp; *iter != 0; ++iter) {
        ++envp_size;
    }

    // Location for new strings.
    char* char_ptr = (char*) sp;

    // Locations for new arrays.
    char** envp_ptr = (char**) sp - envp_size - 1;
    char** argv_ptr = envp_ptr - argv_size - 1;
    int* argc_ptr = (int*) (argv_ptr - 1);

    // Copy argc.
    argc_ptr[0] = argv_size;

    // Copy argv.
    for (unsigned i = 0; i < argv_size; ++i) {
        argv_ptr[i] = char_ptr;
        strcpy(argv_ptr[i], argv[i]);
        char_ptr += strlen(char_ptr) + 1;
    }

    // Copy envp.
    for (unsigned i = 0; i < envp_size; ++i) {
        envp_ptr[i] = char_ptr;
        strcpy(envp_ptr[i], envp[i]);
        char_ptr += strlen(char_ptr) + 1;
    }

    return (void*) argc_ptr;
}
