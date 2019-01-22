#pragma once

#include <fcntl.h>
#include <list.h>

#define S_IFDIR 0040000 // Directory
#define S_IFREG 0100000 // Regular file

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

struct inode {
    mode_t mode;
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
