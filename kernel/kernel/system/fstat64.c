#include <fcntl.h>
#include <fs.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE2(fstat64, int, fd, struct stat *, statbuf)
{
    struct process * process = scheduler_current();

    struct file * file = fs_get_file(process, fd);

    if (file == 0) {
        return -EBADF;
    }

    struct dentry * dentry = file->dentry;

    // @TODO
    statbuf->st_mode = dentry->inode->mode;
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_size = 0;

    return 0;
}
