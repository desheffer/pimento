#include <exec.h>
#include <interrupts.h>
#include <kstdlib.h>
#include <scheduler.h>
#include <task.h>

/**
 * Create a `binprm` to facilitate execution of a new program.
 */
static struct binprm * _binprm_init(const char * pathname, char * const * argv,
                                    char * const * envp)
{
    (void) pathname;
    (void) argv;
    (void) envp;

    struct binprm * binprm = kcalloc(sizeof(struct binprm));

    binprm->mm_context = mm_context_create();

    // @TODO: Copy ELF content into user memory
    // @TODO: Copy `argv` and `envp` into user memory

    unsigned svc[] = { 0xd4000001 };
    mm_copy_to_user(binprm->mm_context, (void *) 0x10, svc, 4);

    binprm->entry = (void *) 0x10;

    return binprm;
}

/**
 * Execute a file, replacing the current task.
 */
void exec(const char * pathname, char * const * argv, char * const * envp)
{
    struct binprm * binprm = _binprm_init(pathname, argv, envp);

    struct task * old_task = scheduler_current_task();
    struct task * task = task_create_binprm(pathname, binprm, old_task->pid);

    interrupts_disable();

    scheduler_replace(old_task, task);

    schedule();
}
