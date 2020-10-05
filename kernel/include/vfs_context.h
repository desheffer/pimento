#pragma once

#include "list.h"
#include "pimento.h"
#include "vfs.h"

struct vfs_context_file {
    struct file * file;
    unsigned fd;
};

struct vfs_context {
    struct list * files;
    unsigned next_fd;
    struct dentry * pwd;
};

struct vfs_context * vfs_context_create(void);
void vfs_context_copy(struct vfs_context *, struct vfs_context *);
int vfs_context_copy_one(struct vfs_context *, struct file *);
void vfs_context_destroy(struct vfs_context *);
struct file * vfs_context_file(struct vfs_context *, unsigned);
int vfs_context_open(struct vfs_context *, struct path *);
