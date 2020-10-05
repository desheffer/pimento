#include <sys/stat.h>

#include "abi.h"
#include "mm_context.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE2(fstat, int, fd, struct stat *, statbuf)
{
    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -EBADF;
    }

    // Create `kstatbuf` in kernel memory.
    // @TODO: Fill in missing stat values.
    struct stat * kstatbuf = kcalloc(sizeof(struct stat));
    kstatbuf->st_mode = file->inode->mode;
    kstatbuf->st_size = file->inode->size;

    mm_copy_to_user(task->mm_context, statbuf, kstatbuf, sizeof(struct stat));

    kfree(kstatbuf);

    return 0;
}
