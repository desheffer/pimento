#include <exec.h>
#include <interrupts.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>
#include <vfs.h>

/**
 * Create a `binprm` to facilitate execution of a new program.
 */
static int _binprm_init(struct binprm * binprm, const char * pathname,
                        char * const * argv, char * const * envp)
{
    (void) argv;
    (void) envp;

    binprm->mm_context = mm_context_create();

    // @TODO: Copy ELF content into user memory
    // @TODO: Copy `argv` and `envp` into user memory

    struct path * path = kcalloc(sizeof(struct path));
    struct file * file = kcalloc(sizeof(struct file));

    vfs_resolve_path(path, vfs_root(), pathname);

    int res = vfs_open(path, file);
    if (res < 0) {
        kfree(path);
        kfree(file);

        return -ENOENT;
    }

    char * bin = kmalloc(EXEC_BUFFER);
    loff_t pos = 0;
    void * entry = (void *) 0x400078;
    char * dest = entry;
    ssize_t num;

    while ((num = vfs_read(file, bin, EXEC_BUFFER, &pos))) {
        mm_copy_to_user(binprm->mm_context, dest, bin, num);
        dest += num;
    }

    binprm->entry = (void *) 0x400078;

    kfree(path);
    kfree(file);
    kfree(bin);

    return 0;
}

/**
 * Execute a file, replacing the current task.
 */
int exec(const char * pathname, char * const * argv, char * const * envp)
{
    struct binprm * binprm = kcalloc(sizeof(struct binprm));

    int res = _binprm_init(binprm, pathname, argv, envp);
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
