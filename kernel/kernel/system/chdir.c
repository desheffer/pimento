#include <fs.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE1(chdir, const char *, path)
{
    struct process * process = scheduler_current();

    struct dentry * dir = fs_dentry(path, process->fs_context->cwd);

    if (dir == 0) {
        return -ENOENT;
    }

    if (dir->inode->type != inode_type_directory) {
        return -ENOTDIR;
    }

    process->fs_context->cwd = dir;

    return 0;
}
