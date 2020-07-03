#include <pimento.h>
#include <vfs.h>
#include <vfs_task.h>

/**
 * Create a virtual file system context for a task.
 */
struct vfs_context * vfs_context_create(void)
{
    struct vfs_context * vfs_context = kcalloc(sizeof(struct vfs_context));

    // Initialize list of open files.
    vfs_context->files = list_create();

    return vfs_context;
}

/**
 * Destroy a virtual file system context for a task.
 */
void vfs_context_destroy(struct vfs_context * vfs_context)
{
    struct vfs_task_file * vfs_task_file;

    while ((vfs_task_file = list_pop_front(vfs_context->files))) {
        if (--vfs_task_file->file->references == 0) {
            vfs_file_destroy(vfs_task_file->file);
        }

        kfree(vfs_task_file);
    }

    list_destroy(vfs_context->files);

    kfree(vfs_context);
}

/**
 * Copy open files.
 */
void vfs_task_copy(struct task * task, struct task * old_task)
{
    list_foreach(old_task->vfs_context->files, struct vfs_task_file *, old_vfs_task_file) {
        struct file * file = old_vfs_task_file->file;

        ++file->references;

        struct vfs_task_file * vfs_task_file = kcalloc(sizeof(struct vfs_task_file));
        vfs_task_file->file = file;
        vfs_task_file->fd = task->vfs_context->next_fd++;

        list_push_back(task->vfs_context->files, vfs_task_file);
    }
}

/**
 * Get an open file for a task.
 */
struct file * vfs_task_file(struct task * task, unsigned fd)
{
    list_foreach(task->vfs_context->files, struct vfs_task_file *, vfs_task_file) {
        if (vfs_task_file->fd == fd) {
            return vfs_task_file->file;
        }
    }

    return 0;
}

/**
 * Open a file for a task.
 */
int vfs_task_open(struct task * task, struct path * path)
{
    struct file * file = vfs_file_create();

    int res = vfs_open(path, file);
    if (res < 0) {
        vfs_file_destroy(file);

        return res;
    }

    struct vfs_task_file * vfs_task_file = kcalloc(sizeof(struct vfs_task_file));
    vfs_task_file->file = file;
    vfs_task_file->fd = task->vfs_context->next_fd++;

    list_push_back(task->vfs_context->files, vfs_task_file);

    return res;
}
