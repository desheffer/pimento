#include <elf.h>
#include <exec.h>
#include <interrupts.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>
#include <vfs.h>

/**
 * Create a `binprm` to facilitate execution of a new program.
 */
static int _binprm_load(struct binprm * binprm, const char * pathname,
                        char * const * argv, char * const * envp)
{
    (void) argv;
    (void) envp;

    binprm->mm_context = mm_context_create();

    // @TODO: Copy `argv` and `envp` into user memory

    struct path * path = vfs_path_create();
    struct file * file = vfs_file_create();

    vfs_resolve_path(path, vfs_root(), pathname);

    // Open the executable file.
    // @TODO: Check for executable bit.
    int res = vfs_open(path, file);
    if (res < 0) {
        vfs_path_destroy(path);
        vfs_file_destroy(file);

        return res;
    }

    // Load using the ELF format.
    res = elf_load(binprm, file);
    if (res < 0) {
        vfs_path_destroy(path);
        vfs_file_destroy(file);

        return res;
    }

    vfs_path_destroy(path);
    vfs_file_destroy(file);

    return 0;
}

/**
 * Execute a file, replacing the current task.
 */
int exec(const char * pathname, char * const * argv, char * const * envp)
{
    struct binprm * binprm = kcalloc(sizeof(struct binprm));

    int res = _binprm_load(binprm, pathname, argv, envp);
    if (res < 0) {
        return res;
    }

    struct task * old_task = scheduler_current_task();
    struct task * task = task_create_binprm(pathname, binprm, old_task->pid);

    interrupts_disable();

    scheduler_replace(old_task, task);

    schedule();

    return 0;
}
