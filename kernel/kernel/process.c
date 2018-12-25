#include <list.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

process_t* process_create_kernel()
{
    // Create a new process control block.
    process_t* process = malloc(sizeof(process_t));
    memset(process, 0, sizeof(process_t));

    // Assign a pid and basic information.
    process->pid = 0;
    process->state = running;
    strncpy(process->pname, "kernel", PNAME_LENGTH);

    // Initialize list of allocated pages.
    process->pages = list_new();

    // Initialize memory map.
    mmap_create(process);

    // Initialize execution.
    process->cpu_context = malloc(sizeof(cpu_context_t));

    return process;
}

process_t* process_create(const char* pname, const void* fn, const void* data)
{
    // Create a new process control block.
    process_t* process = malloc(sizeof(process_t));
    memset(process, 0, sizeof(process_t));

    // Assign a pid and basic information.
    process->pid = scheduler_assign_pid();
    process->state = created;
    strncpy(process->pname, pname, PNAME_LENGTH);

    // Initialize list of allocated pages.
    process->pages = list_new();

    // Initialize memory map.
    mmap_create(process);

    // Allocate the first page of the interrupt stack.
    void* int_stack_top = (char*) alloc_user_page(process) + PAGE_SIZE;

    // Initialize execution.
    process->cpu_context = malloc(sizeof(cpu_context_t));
    process->cpu_context->regs[0] = (long unsigned) fn;
    process->cpu_context->regs[1] = (long unsigned) data;
    process->cpu_context->sp = (long unsigned) int_stack_top;
    process->cpu_context->pc = (long unsigned) fork_tail;

    // Add it to the scheduling queue.
    scheduler_enqueue(process);

    return process;
}

void process_destroy(process_t* process)
{
    (void) process;

    // @TODO: Call free_user_page() for each entry in process->pages.
    // @TODO: Free process.
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
