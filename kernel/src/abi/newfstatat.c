#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <sys/stat.h>
#include <task.h>
#include <vfs_task.h>

SYSCALL_DEFINE4(newfstatat, int, dirfd, const char *, pathname, struct stat *, statbuf, int, flags)
{
    struct task * task = scheduler_current_task();

    if (dirfd != -100) {
        return -EINVAL;
    }

    if (flags != 0) {
        return -EINVAL;
    }

    struct path * path = vfs_path_create();

    char * kpathname = 0;
    if (pathname != 0) {
        kpathname = kmalloc(strnlen(pathname, page_size()));
        mm_copy_from_user(task->mm_context, kpathname, pathname, page_size());
    }

    vfs_resolve_path(path, task->vfs_context->pwd, kpathname);

    if (path->child == 0) {
        vfs_path_destroy(path);
        kfree(kpathname);

        return -ENOENT;
    }

    // @TODO: Copy statbuf.
    (void) statbuf;

    vfs_path_destroy(path);
    kfree(kpathname);

    return 0;
}
