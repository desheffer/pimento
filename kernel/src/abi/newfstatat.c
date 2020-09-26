#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <sys/stat.h>
#include <task.h>
#include <vfs_task.h>

SYSCALL_DEFINE4(newfstatat, int, dirfd, const char *, pathname, struct stat *, statbuf, int, flags)
{
    if (dirfd != -100) {
        return -EINVAL;
    }

    if (flags != 0) {
        return -EINVAL;
    }

    struct task * task = scheduler_current_task();

    struct path * path = vfs_path_create();

    vfs_resolve_path(path, task->vfs_context->pwd, pathname);

    if (path->child == 0) {
        vfs_path_destroy(path);

        return -ENOENT;
    }

    // @TODO: Copy statbuf.
    (void) statbuf;

    vfs_path_destroy(path);

    return 0;
}
