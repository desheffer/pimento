#include <fs.h>
#include <scheduler.h>
#include <string.h>
#include <system.h>

SYSCALL_DEFINE3(getdents64, unsigned, fd, struct dirent *, buf, unsigned, count)
{
    struct process * process = scheduler_current();

    struct file * file = fs_get_file(process, fd);
    if (file == 0) {
        return -1;
    }

    struct list_item * subdir_item = file->dentry->dentry_children->front;
    unsigned pos = file->pos++;

    while (subdir_item != 0) {
        if (pos-- == 0) {
            struct dentry * subdir = (struct dentry *) subdir_item->item;

            buf->d_reclen = strlen(subdir->name);
            strncpy(buf->d_name, subdir->name, count);

            return 1;
        }

        subdir_item = subdir_item->next;
    }

    return -1;
}
