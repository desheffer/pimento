#include "abi.h"
#include "abi/string.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs.h"
#include "vfs_context.h"

SYSCALL_DEFINE2(getcwd, char *, buf, size_t, count)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct dentry * pwd = task->vfs_context->pwd;
    struct dentry * iter_parent = vfs_root();
    struct dentry * iter_child = 0;

    while (iter_parent != 0) {
        // Copy the name of the parent directory.
        res += strncpy_to_user(task->mm_context, buf + res, iter_parent->name, count - res);

        if (iter_parent == pwd) {
            break;
        }

        // Add a slash before everything but the root directory.
        if (iter_child != 0) {
            res += strncpy_to_user(task->mm_context, buf + res, "/", count - res);
        }

        // Find the next directory in the path.
        iter_child = pwd;
        while (iter_child->parent != iter_parent) {
            iter_child = iter_child->parent;
        }

        iter_parent = iter_child;
    }

    return res;
}
