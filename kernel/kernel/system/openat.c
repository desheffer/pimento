#include <assert.h>
#include <fs.h>
#include <kstdlib.h>
#include <scheduler.h>
#include <string.h>
#include <synchronize.h>
#include <system.h>

SYSCALL_DEFINE4(openat, int, dirfd, const char *, pathname, int, flags, mode_t, mode)
{
    // @TODO
    (void) flags;
    (void) mode;

    struct process * process = scheduler_current();

    struct dentry * cwd;

    if (dirfd == AT_FDCWD) {
        cwd = process->fs_context->cwd;
    } else {
        struct file * file = fs_get_file(process, dirfd);

        if (file == 0) {
            return -EBADF;
        }

        cwd = file->dentry;
    }

    struct dentry * dentry = fs_dentry(pathname, cwd);

    if (dentry == 0) {
        return -ENOENT;
    }

    struct file * file = kzalloc(sizeof(struct file));
    file->dentry = dentry;
    file->pos = 0;

    enter_critical();

    file->fd = process->fs_context->next_fd++;

    list_push_back(process->fs_context->file_list, file);

    leave_critical();

    return file->fd;
}
