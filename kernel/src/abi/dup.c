#include "abi.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE1(dup, int, fd)
{
    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -EBADF;
    }

    int new_fd = vfs_context_copy_one(task->vfs_context, file);

    return new_fd;
}
