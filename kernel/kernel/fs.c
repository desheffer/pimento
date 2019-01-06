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

void fs_process_create(struct process * process)
{
    process->fs_context = kzalloc(sizeof(struct process));
    process->fs_context->root = _dentry_root;
    // @TODO: Copy cwd from parent process.
    process->fs_context->cwd = _dentry_root;
}

void fs_process_destroy(struct process * process)
{
    kfree(process->fs_context);
}
