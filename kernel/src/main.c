#include "exec.h"
#include "fs/devfs.h"
#include "initrd.h"
#include "modules.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs.h"
#include "vfs_context.h"

static void _open_io(void)
{
    struct path * path = vfs_path_create();
    struct task * task = scheduler_current_task();

    vfs_resolve_path(path, 0, "/dev/ttyS0");

    vfs_context_open(task->vfs_context, path); // stdin  (fd = 0)
    vfs_context_open(task->vfs_context, path); // stdout (fd = 1)
    vfs_context_open(task->vfs_context, path); // stderr (fd = 2)

    vfs_path_destroy(path);
}

/**
 * Execute the init program.
 */
static void _exec_init(struct task * init_task)
{
    _open_io();

    const char * const argv[] = {"/bin/sh", 0};
    const char * const envp[] = {"HOME=/", 0};
    exec(init_task, "/bin/sh", (char * const *) argv, (char * const *) envp);

    schedule();

    fail("Failed to start init.");
}

/**
 * Finish kernel initialization after all architecture-specific steps have been
 * completed.
 */
void kernel_main(void)
{
    schedule();

    struct task * init_task = scheduler_current_task();

    vfs_init();
    initrd_init(init_task->vfs_context);
    devfs_init();
    modules_init();

    kputs(WELCOME_MSG);

    _exec_init(init_task);
}
