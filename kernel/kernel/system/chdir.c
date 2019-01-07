#include <fs.h>
#include <scheduler.h>
#include <string.h>
#include <system.h>

SYSCALL_DEFINE1(chdir, const char *, path)
{
    struct process * process = scheduler_current();

    struct dentry * dir = process->fs_context->cwd;

    if (strcmp("..", path) == 0) {
        if (dir->dentry_parent == 0) {
            return -1;
        }

        process->fs_context->cwd = dir->dentry_parent;

        return 0;
    }

    struct list_item * subdir_item = dir->dentry_children->front;

    while (subdir_item != 0) {
        struct dentry * subdir = (struct dentry *) subdir_item->item;

        if (strcmp(subdir->name, path) == 0) {
            process->fs_context->cwd = subdir;

            return 0;
        }

        subdir_item = subdir_item->next;
    }

    return -1;
}
