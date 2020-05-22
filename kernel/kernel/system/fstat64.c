#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <scheduler.h>
#include <string.h>
#include <sys/stat.h>
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
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = dentry->inode->mode;

    return 0;
}
