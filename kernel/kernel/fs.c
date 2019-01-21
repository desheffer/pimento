#include <fs.h>
#include <kstdlib.h>
#include <ramfs.h>
#include <string.h>
#include <vfs.h>

static struct dentry * _dentry_root = 0;
static struct vfsmount * _mounts = 0;

void fs_init(void)
{
    _dentry_root = ramfs_create();
    _mounts = kzalloc(sizeof(struct vfsmount));
}

static struct dentry * fs_dentry_single(const char * path, struct dentry * cwd)
{
    if (strcmp("..", path) == 0) {
        if (cwd->dentry_parent == 0) {
            return 0;
        }

        return cwd->dentry_parent;
    }

    struct list_item * subdir_item = cwd->dentry_children->front;

    while (subdir_item != 0) {
        struct dentry * subdir = (struct dentry *) subdir_item->item;

        if (strcmp(subdir->name, path) == 0) {
            return subdir;
        }

        subdir_item = subdir_item->next;
    }

    return 0;
}

struct dentry * fs_dentry(const char * path, struct dentry * cwd)
{
    if (path[0] == '/') {
        cwd = _dentry_root;
        ++path;
    }

    if (cwd == 0) {
        return 0;
    }

    char * kpath = kmalloc(strlen(path) + 1);
    strcpy(kpath, path);

    char * saveptr = 0;
    char * token = strtok_r(kpath, "/", &saveptr);
    while (token != 0) {
        cwd = fs_dentry_single(token, cwd);
        if (cwd == 0) {
            return 0;
        }

        token = strtok_r(0, "/", &saveptr);
    }

    kfree(kpath);

    return cwd;
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
