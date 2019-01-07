#include <assert.h>
#include <fs.h>
#include <kstdlib.h>
#include <scheduler.h>
#include <string.h>
#include <synchronize.h>
#include <system.h>

SYSCALL_DEFINE4(openat, int, dirfd, const char *, pathname, int, flags, mode_t, mode)
{
    failif(dirfd != -100);
    failif(strcmp(".", pathname) != 0);
    failif(flags != 0xA4000);
    failif(mode != 0);

    struct process * process = scheduler_current();

    struct file * file = kzalloc(sizeof(struct file));
    file->dentry = process->fs_context->cwd;
    file->pos = 0;

    enter_critical();

    file->fd = process->fs_context->next_fd++;

    list_push_back(process->fs_context->file_list, file);

    leave_critical();

    return file->fd;
}
