#include <assert.h>
#include <elf.h>
#include <kstdlib.h>
#include <memory.h>
#include <mm.h>
#include <process.h>
#include <scheduler.h>
#include <stdbool.h>
#include <string.h>
#include <synchronize.h>

extern char __shell_start;
extern char __shell_end;

static process_t* process_create_common(const char* pname, int pid, bool init_kstack, bool enqueue)
{
    // Create a new process control block.
    process_t* child = kzalloc(sizeof(process_t));

    // Assign a pid.
    if (pid >= 0) {
        child->pid = pid;
    } else {
        child->pid = scheduler_assign_pid();
    }

    // Assign basic information.
    child->state = created;
    strncpy(child->pname, pname, PNAME_LENGTH);

    // Initialize a new memory map.
    mm_create(child);
    if (init_kstack) {
        mm_create_kstack(child);
    }

    // Initialize execution.
    child->cpu_context = kzalloc(sizeof(cpu_context_t));

    // Enqueue for scheduling.
    if (enqueue) {
        scheduler_enqueue(child);
    }

    return child;
}

process_t* process_create_kernel(void)
{
    process_t* process = process_create_common("kernel", -1, false, false);

    return process;
}

void process_destroy(process_t* process)
{
    (void) process;

    // @TODO: Free process and all linked data.
}

int process_create(void* fn, const char* pname, void* data)
{
    enter_critical();

    process_t* child = process_create_common(pname, -1, true, true);

    // Initialize execution.
    child->cpu_context->sp = (long unsigned) KSTACK_TOP;
    child->cpu_context->pc = (long unsigned) process_create_tail_wrapper;
    child->cpu_context->regs[0] = (long unsigned) fn;
    child->cpu_context->regs[1] = (long unsigned) data;

    leave_critical();

    return child->pid;
}

void process_create_tail(process_function_t fn, void* data)
{
    scheduler_tail();

    fn(data);

    enter_critical();

    // Exit the process when it is finished.
    process_t* process = scheduler_current();
    process->state = zombie;

    leave_critical();

    scheduler_context_switch();
}

int process_clone(process_t* parent)
{
    enter_critical();

    process_t* child = process_create_common(parent->pname, -1, false, true);

    // Copy the memory map.
    mm_copy_from(parent, child);

    // Initialize execution.
    child->cpu_context->sp = (long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE;
    child->cpu_context->pc = (long unsigned) process_create_tail_wrapper;
    child->cpu_context->regs[0] = (long unsigned) do_exec;
    child->cpu_context->regs[1] = (long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE;

    leave_critical();

    return child->pid;
}

int process_exec(const char* pname, char* const argv[], char* const envp[])
{
    // @TODO: Support arbitrary files.
    failif(strcmp("/bin/sh", pname) != 0);

    enter_critical();

    process_t* parent = scheduler_current();

    process_t* child = process_create_common(pname, parent->pid, true, true);

    // Initialize execution.
    child->cpu_context->sp = (long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE;
    child->cpu_context->pc = (long unsigned) process_exec_tail_wrapper;
    child->cpu_context->regs[0] = (long unsigned) pname;
    child->cpu_context->regs[1] = (long unsigned) argv;
    child->cpu_context->regs[2] = (long unsigned) envp;

    parent->state = zombie;

    leave_critical();

    scheduler_context_switch();

    return -1;
}

void process_exec_tail(const char* pname, char* const* argv, char* const* envp)
{
    (void) pname;

    scheduler_tail();

    // Load child stack with argv and envp.
    // @TODO: Copy args to kernel memory.
    void* stack_top = (void*) USTACK_TOP;
    stack_top = process_set_args(stack_top, argv, envp);

    // Initialize process.
    registers_t* new_regs = (registers_t*) ((long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE);
    new_regs->sp = (long unsigned) stack_top;
    new_regs->pc = (long unsigned) elf_load(&__shell_start, &__shell_end - &__shell_start);
    new_regs->pstate = PSR_MODE_USER;

    do_exec(new_regs);

    scheduler_context_switch();
}

void* process_set_args(void* sp, char* const argv[], char* const envp[])
{
    // @TODO: Add limits on array and string lengths.

    // Location for new strings.
    char* char_ptr = (char*) sp;

    // Find size of argv.
    unsigned argv_size = 0;
    for (char* const* iter = argv; *iter != 0; ++iter) {
        ++argv_size;
        char_ptr = char_ptr - strlen(*iter) - 1;
    }

    // Find size of envp.
    unsigned envp_size = 0;
    for (char* const* iter = envp; *iter != 0; ++iter) {
        ++envp_size;
        char_ptr = char_ptr - strlen(*iter) - 1;
    }

    // Adjust stack alignment.
    char_ptr = (char*) STACK_ALIGN((long unsigned) char_ptr);

    // Locations for new arrays.
    char** envp_ptr = (char**) char_ptr - envp_size - 1;
    char** argv_ptr = (char**) envp_ptr - argv_size - 1;
    int* argc_ptr = (int*) ((char**) argv_ptr - 1);

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
