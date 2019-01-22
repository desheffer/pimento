#pragma once

#include <list.h>
#include <process.h>
#include <vfs.h>

struct fs_context {
    struct dentry * root;
    struct dentry * cwd;
    unsigned next_fd;
    struct list * file_list;
};

void fs_init(void);
struct dentry * fs_dentry(const char *, struct dentry *);
struct file * fs_get_file(struct process *, unsigned);
void fs_process_create(struct process *, struct process *);
void fs_process_destroy(struct process *);
