#pragma once

#include <list.h>
#include <task.h>
#include <vfs.h>

struct vfs_task_file {
    struct file * file;
    unsigned fd;
};

struct vfs_context {
    struct list * files;
    unsigned next_fd;
    struct dentry * pwd;
};

void vfs_task_copy(struct task *, struct task *);
struct file * vfs_task_file(struct task *, unsigned);
int vfs_task_open(struct task *, struct path *);
