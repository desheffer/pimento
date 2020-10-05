#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs.h"
#include "vfs_context.h"

SYSCALL_DEFINE1(chdir, const char *, pathname)
{
    struct task * task = scheduler_current_task();

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

    if ((path->child->inode->mode & INODE_IFDIR) == 0) {
        return -ENOTDIR;
    }

    task->vfs_context->pwd = path->child;

    return 0;
}
