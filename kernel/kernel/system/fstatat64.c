#include <fcntl.h>
#include <fs.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE4(fstatat64, int, dirfd, const char *, pathname, struct stat *, statbuf, int, flags)
{
    // @TODO
    (void) statbuf;
    (void) flags;

    struct process * process = scheduler_current();

    struct dentry * cwd;

    if (dirfd == AT_FDCWD) {
        cwd = process->fs_context->cwd;
    } else {
        // @TODO
        return -ENOENT;
    }

    struct dentry * file = fs_dentry(pathname, cwd);

    if (file == 0) {
        return -ENOENT;
    }

    // @TODO
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_size = 0;

    return 0;
}
