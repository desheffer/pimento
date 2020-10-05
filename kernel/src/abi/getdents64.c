#include <dirent.h>

#include "abi.h"
#include "mm_context.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE3(getdents64, unsigned, fd, struct dirent *, buf, unsigned, count)
{
    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -EBADF;
    }

    struct dentry * parent = list_peek_front(file->inode->dentries);
    struct dentry * child = list_at(parent->children, file->pos);
    if (child == 0) {
        return -EINVAL;
    }

    // Create `kbuf` in kernel memory.
    // @TODO: Fill in missing stat values.
    struct dirent * kbuf = kcalloc(sizeof(struct dirent));
    kbuf->d_reclen = count;
    strncpy(kbuf->d_name, child->name, count - 2 - 3 * 4);

    mm_copy_to_user(task->mm_context, buf, kbuf, count);

    kfree(kbuf);

    ++file->pos;

    return 1;
}
