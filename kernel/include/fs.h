#pragma once

#include <process.h>
#include <vfs.h>

struct fs_context {
    struct dentry * root;
    struct dentry * cwd;
};

void fs_init(void);
void fs_process_create(struct process *);
void fs_process_destroy(struct process *);
