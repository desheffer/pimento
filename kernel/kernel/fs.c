#include <fs.h>
#include <kstdlib.h>
#include <ramfs.h>
#include <vfs.h>

static struct dentry * _dentry_root = 0;
static struct vfsmount * _mounts = 0;

void fs_init(void)
{
    _dentry_root = ramfs_create();
    _mounts = kzalloc(sizeof(struct vfsmount));
}

struct file * fs_get_file(struct process * process, unsigned fd)
{
    struct list_item * file_item = process->fs_context->file_list->front;

    while (file_item != 0) {
        struct file * file = (struct file *) file_item->item;
        if (file->fd == fd) {
            return file;
        }

        file_item = file_item->next;
    }

    return 0;
}

void fs_process_create(struct process * process)
{
    process->fs_context = kzalloc(sizeof(struct process));
    process->fs_context->root = _dentry_root;
    // @TODO: Copy cwd from parent process.
    process->fs_context->cwd = _dentry_root;
    process->fs_context->file_list = list_create();

    // Reserve stdin, stdout, and stderr.
    // @TODO: Allocate these as files.
    process->fs_context->next_fd = 3;
}

void fs_process_destroy(struct process * process)
{
    list_destroy(process->fs_context->file_list);
    kfree(process->fs_context);
}
