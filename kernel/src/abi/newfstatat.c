#include <sys/stat.h>

#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE4(newfstatat, int, dirfd, const char *, pathname, struct stat *, statbuf, int, flags)
{
    struct task * task = scheduler_current_task();

    if (dirfd != -100) {
        return -EINVAL;
    }

    if (flags != 0x0 && flags != 0x100) {
        return -EINVAL;
    }

    struct path * path = vfs_path_create();

    // Copy `pathname` to kernel memory.
    char * kpathname = 0;
    if (pathname != 0) {
        size_t length = strnlen(pathname, page_size());
        kpathname = kmalloc(strnlen(pathname, page_size()));
        mm_copy_from_user(task->mm_context, kpathname, pathname, length);
    }

    vfs_resolve_path(path, task->vfs_context->pwd, kpathname);

    if (path->child == 0) {
        vfs_path_destroy(path);
        kfree(kpathname);

        return -ENOENT;
    }

    // Create `kstatbuf` in kernel memory.
    // @TODO: Fill in missing stat values.
    struct stat * kstatbuf = kmalloc(sizeof(struct stat));
    mm_copy_from_user(task->mm_context, kstatbuf, statbuf, sizeof(*statbuf));
    /* kstatbuf->st_mode = path->child->inode->mode; */
    /* kstatbuf->st_size = path->child->inode->size; */

    mm_copy_to_user(task->mm_context, statbuf, kstatbuf, sizeof(*statbuf));

    vfs_path_destroy(path);
    kfree(kpathname);
    kfree(kstatbuf);

    return 0;
}
