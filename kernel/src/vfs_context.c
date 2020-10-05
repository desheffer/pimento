#include "list.h"
#include "pimento.h"
#include "vfs.h"
#include "vfs_context.h"

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
 * Copy open files.
 */
void vfs_context_copy(struct vfs_context * vfs_context,
                      struct vfs_context * old_vfs_context)
{
    list_foreach(old_vfs_context->files, struct vfs_context_file *, old_vfs_context_file) {
        struct file * file = old_vfs_context_file->file;

        ++file->references;

        struct vfs_context_file * vfs_context_file = kcalloc(sizeof(struct vfs_context_file));
        vfs_context_file->file = file;
        vfs_context_file->fd = vfs_context->next_fd++;

        list_push_back(vfs_context->files, vfs_context_file);
    }

    vfs_context->pwd = old_vfs_context->pwd;
}

/**
 * Duplicate an open file.
 */
int vfs_context_copy_one(struct vfs_context * vfs_context, struct file * file)
{
    ++file->references;

    struct vfs_context_file * vfs_context_file = kcalloc(sizeof(struct vfs_context_file));
    vfs_context_file->file = file;
    vfs_context_file->fd = vfs_context->next_fd++;

    list_push_back(vfs_context->files, vfs_context_file);

    return vfs_context_file->fd;
}

/**
 * Destroy a virtual file system context for a task.
 */
void vfs_context_destroy(struct vfs_context * vfs_context)
{
    struct vfs_context_file * vfs_context_file;

    while ((vfs_context_file = list_pop_front(vfs_context->files))) {
        if (--vfs_context_file->file->references == 0) {
            vfs_file_destroy(vfs_context_file->file);
        }

        kfree(vfs_context_file);
    }

    list_destroy(vfs_context->files);

    kfree(vfs_context);
}

/**
 * Get an open file for a task.
 */
struct file * vfs_context_file(struct vfs_context * vfs_context, unsigned fd)
{
    list_foreach(vfs_context->files, struct vfs_context_file *, vfs_context_file) {
        if (vfs_context_file->fd == fd) {
            return vfs_context_file->file;
        }
    }

    return 0;
}

/**
 * Open a file for a task.
 */
int vfs_context_open(struct vfs_context * vfs_context, struct path * path)
{
    struct file * file = vfs_file_create();
    ++file->references;

    int res = vfs_open(path, file);
    if (res < 0) {
        vfs_file_destroy(file);

        return res;
    }

    struct vfs_context_file * vfs_context_file = kcalloc(sizeof(struct vfs_context_file));
    vfs_context_file->file = file;
    vfs_context_file->fd = vfs_context->next_fd++;

    list_push_back(vfs_context->files, vfs_context_file);

    return vfs_context_file->fd;
}
