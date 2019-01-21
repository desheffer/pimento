#pragma once

#include <list.h>

struct vfsmount {
    struct dentry * dentry_mountpoint;
    struct dentry * dentry_root;
};

enum file_system_type {
    file_system_type_ramfs,
    file_system_type_devicefs,
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

enum inode_type {
    inode_type_directory,
    inode_type_file,
};

struct inode {
    enum inode_type type;
    struct superblock * superblock;
    struct file_operations * file_operations;
    struct inode_operations * inode_operations;
};

struct inode_operations {
};

struct file {
    unsigned fd;
    struct dentry * dentry;
    unsigned pos;
    struct file_operations * file_operations;
};

struct file_operations {
};
