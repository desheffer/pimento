#pragma once

#include <list.h>

struct vfsmount {
    struct dentry * dentry_mountpoint;
    struct dentry * dentry_root;
};

enum file_system_type {
    ramfs,
    devicefs,
};

struct superblock {
    enum file_system_type type;
    struct dentry * dentry_root;
    struct superblock_operations * superblock_operations;
};

struct superblock_operations {
};

struct dentry {
    struct inode * inode;
    struct dentry * dentry_parent;
    const char * name;
    struct superblock * superblock;
    struct list * dentry_children;
    struct dentry_operations * dentry_operations;
};

struct dentry_operations {
};

struct inode {
    struct superblock * superblock;
    struct file_operations * file_operations;
    struct inode_operations * inode_operations;
};

struct inode_operations {
};

struct file {
    struct dentry * dentry;
    struct file_operations * file_operations;
};

struct file_operations {
};
