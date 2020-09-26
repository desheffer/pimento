#include <exec.h>
#include <fs/devfs.h>
#include <initrd.h>
#include <modules.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>
#include <vfs.h>
#include <vfs_task.h>

static void _open_io(void)
{
    struct path * path = vfs_path_create();
    struct task * task = scheduler_current_task();

    vfs_resolve_path(path, 0, "/dev/ttyS0");

    vfs_task_open(task, path); // stdin  (fd = 0)
    vfs_task_open(task, path); // stdout (fd = 1)
    vfs_task_open(task, path); // stderr (fd = 2)

    vfs_path_destroy(path);
}

/**
 * Execute the init program.
 */
static void _exec_init(void)
{
    const char * const argv[] = {"/bin/sh", 0};
    const char * const envp[] = {"HOME=/", 0};

    _open_io();
    exec("/bin/sh", (char * const *) argv, (char * const *) envp);

    fail("Failed to start init.");
}

/**
 * Finish kernel initialization after all architecture-specific steps have been
 * completed.
 */
void kernel_main(void)
{
    schedule();

    vfs_init();
    initrd_init();
    devfs_init();
    modules_init();

    kputs(WELCOME_MSG);

    _exec_init();
}
