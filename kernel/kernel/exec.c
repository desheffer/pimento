#include <asm/boot.h>
#include <assert.h>
#include <elf.h>
#include <exec.h>
#include <kstdlib.h>
#include <mm.h>
#include <process.h>
#include <scheduler.h>
#include <string.h>
#include <synchronize.h>

extern char __shell_start;
extern char __shell_end;

static char ** copy_args(char * const * args)
{
    // Find args count.
    unsigned args_count = 0;
    for (char * const * iter = args; *iter != 0; ++iter) {
        ++args_count;
    }

    char ** new_args = kmalloc((args_count + 1) * sizeof(char **));
    char ** new_args_iter = new_args;

    // Copy args.
    for (char * const * iter = args; *iter != 0; ++iter) {
        char * new_arg = kmalloc(strlen(*iter) + 1);
        strcpy(new_arg, *iter);
        *(new_args_iter++) = new_arg;
    }
    *new_args_iter = 0;

    return new_args;
}

static void * set_args(void * sp, char * const * argv, char * const * envp)
{
    // @TODO: Add limits on array and string lengths.

    // Location for new strings.
    char * char_ptr = (char *) sp;

    // Find size of argv and elements.
    unsigned argv_size = 0;
    for (char * const * iter = argv; *iter != 0; ++iter) {
        ++argv_size;
        char_ptr = char_ptr - strlen(*iter) - 1;
    }

    // Find size of envp and elements.
    unsigned envp_size = 0;
    for (char * const * iter = envp; *iter != 0; ++iter) {
        ++envp_size;
        char_ptr = char_ptr - strlen(*iter) - 1;
    }

    // Size of auxv.
    unsigned auxv_size = 1;

    // Adjust stack alignment.
    char_ptr = (char *) STACK_ALIGN((long unsigned) char_ptr);

    // Locations for new arrays.
    long unsigned * auxv_ptr = (long unsigned *) char_ptr - (auxv_size * 2) - 1;
    char ** envp_ptr = (char **) auxv_ptr - envp_size - 1;
    char ** argv_ptr = (char **) envp_ptr - argv_size - 1;
    int * argc_ptr = (int *) ((char **) argv_ptr - 1);

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

    // Set auxv.
    auxv_ptr[0] = AT_PAGESZ;
    auxv_ptr[1] = PAGE_SIZE;

    return (void *) argc_ptr;
}

static void process_exec_tail(struct binprm * bprm)
{
    scheduler_tail();

    // Load child stack with argv and envp.
    void * sp = (void *) USTACK_TOP;
    sp = set_args(sp, bprm->argv, bprm->envp);

    // Initialize process.
    struct registers * new_regs = (struct registers *) ((long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE);
    new_regs->sp = (long unsigned) sp;
    new_regs->pc = (long unsigned) elf_load(&__shell_start, &__shell_end - &__shell_start);
    new_regs->pstate = PSR_MODE_USER;

    kfree(bprm->filename);
    kfree(bprm->argv);
    kfree(bprm->envp);
    kfree(bprm);

    do_exec(new_regs);
}

int process_exec(const char * pname, char * const * argv, char * const * envp)
{
    enter_critical();

    // @TODO: Support arbitrary files.
    failif(strcmp("/bin/sh", pname) != 0);

    struct process * parent = scheduler_current();

    struct process * child = process_create_common(pname, parent->pid, true);

    struct binprm * bprm = kzalloc(sizeof(struct binprm));

    bprm->filename = kmalloc(strlen(pname) + 1);
    strcpy(bprm->filename, pname);

    bprm->argv = copy_args(argv);
    bprm->envp = copy_args(envp);

    // Initialize execution.
    child->cpu_context->sp = (long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE;
    child->cpu_context->pc = (long unsigned) process_tail_wrapper;
    child->cpu_context->regs[0] = (long unsigned) process_exec_tail;
    child->cpu_context->regs[1] = (long unsigned) bprm;

    scheduler_enqueue(child);
    scheduler_exit(parent);

    leave_critical();

    return child->pid;
}
