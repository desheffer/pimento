#include <list.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

process_t* process_create(const char* pname, const void* fn, const void* data)
{
    enter_critical();

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

    // Allocate the first page of the stack.
    void* stack_begin = alloc_user_page(process);
    mmap_map_page(process, (void*) (STACK_TOP - PAGE_SIZE), stack_begin);
    void* stack_end = (char*) stack_begin + PAGE_SIZE;

    // Initialize startup parameters.
    process_regs_t* regs = (process_regs_t*) stack_end - 1;
    regs->pstate = PSR_MODE_KTHREAD;
    regs->pc = (long unsigned) fn;
    regs->regs[0] = (long unsigned) data;
    process->sp = regs;

    // Add it to the process list and scheduling queue.
    scheduler_enqueue(process);

    leave_critical();

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
