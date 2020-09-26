#include <elf.h>
#include <exec.h>
#include <interrupts.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>
#include <vfs.h>
#include <vfs_task.h>

/**
 * Count the number of elements in a string array.
 */
static unsigned _binprm_args_count(char * const * args)
{
    unsigned args_count = 0;
    for (char * const * iter = args; *iter != 0; ++iter) {
        ++args_count;
    }

    return args_count;
}

/**
 * Calculate the total size of strings in a string array.
 */
static size_t _binprm_args_str_size(char * const * args)
{
    size_t args_size = 0;
    for (char * const * iter = args; *iter != 0; ++iter) {
        args_size += strlen(*iter) + 1;
    }

    return args_size;
}

/**
 * Allocate memory in the given `binprm`.
 */
static void * _binprm_calloc(struct binprm * binprm, size_t size)
{
    binprm->stack_bottom = (char *) binprm->stack_bottom - size;

    binprm->stack_bottom = (void *) ((uintptr_t) binprm->stack_bottom & ~0xFF);

    return binprm->stack_bottom;
}

/**
 * Copy auxv values into the given `binprm`.
 */
static void _binprm_auxv(struct binprm * binprm, long unsigned * new_auxv)
{
    size_t auxv_size = sizeof(long unsigned) * (AUXV_COUNT * 2 + 1);

    long unsigned auxv[] = {
        AT_PAGESZ,
        page_size(),
        AT_NULL,
    };

    mm_copy_to_user(binprm->mm_context, new_auxv, auxv, auxv_size);
}

/**
 * Copy runtime arguments into place, beginning at `binprm->stack_bottom`.
 */
static void _binprm_copy_args(struct binprm * binprm, char * const * argv,
                              char * const * envp)
{
    unsigned argv_count = _binprm_args_count(argv);
    unsigned envp_count = _binprm_args_count(envp);

    size_t argc_size = sizeof(long unsigned);
    size_t argv_size = sizeof(char *) * (argv_count + 1);
    size_t envp_size = sizeof(char *) * (envp_count + 1);
    size_t auxv_size = sizeof(long unsigned) * (AUXV_COUNT * 2 + 1);

    size_t argv_str_size = sizeof(char) * _binprm_args_str_size(argv);
    size_t envp_str_size = sizeof(char) * _binprm_args_str_size(envp);

    char * new_envp_str = _binprm_calloc(binprm, envp_str_size);
    char * new_argv_str = _binprm_calloc(binprm, argv_str_size);

    int * new_argc = _binprm_calloc(binprm, argc_size + argv_size + envp_size + auxv_size);
    char ** new_argv = (char **) ((uintptr_t) new_argc + argc_size);
    char ** new_envp = (char **) ((uintptr_t) new_argv + argv_size);
    long unsigned * new_auxv = (long unsigned *) ((uintptr_t) new_envp + envp_size);

    // Copy argc.
    mm_copy_to_user(binprm->mm_context, new_argc, &argv_count, sizeof(int));

    // Copy argv.
    for (unsigned i = 0; i < argv_count; ++i) {
        unsigned length = strlen(argv[i]);
        mm_copy_to_user(binprm->mm_context, new_argv_str, argv[i], length + 1);
        mm_copy_to_user(binprm->mm_context, &new_argv[i], &new_argv_str, sizeof(char *));
        new_argv_str += length + 1;
    }

    // Copy envp.
    for (unsigned i = 0; i < envp_count; ++i) {
        unsigned length = strlen(envp[i]);
        mm_copy_to_user(binprm->mm_context, new_envp_str, envp[i], length + 1);
        mm_copy_to_user(binprm->mm_context, &new_envp[i], &new_envp_str, sizeof(char *));
        new_envp_str += length + 1;
    }

    // Copy auxv.
    _binprm_auxv(binprm, new_auxv);
}

/**
 * Create a `binprm` to facilitate execution of a new program.
 */
static int _binprm_load(struct binprm * binprm, const char * pathname,
                        char * const * argv, char * const * envp)
{
    binprm->mm_context = mm_context_create_user();

    // Copy arguments into user memory.
    binprm->stack_bottom = mm_context_stack_top(binprm->mm_context);
    _binprm_copy_args(binprm, argv, envp);

    struct path * path = vfs_path_create();
    struct file * file = vfs_file_create();

    vfs_resolve_path(path, 0, pathname);

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

    vfs_task_copy(task, old_task);

    interrupts_disable();

    scheduler_replace(old_task, task);

    schedule();

    return 0;
}
