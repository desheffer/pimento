#pragma once

#include <list.h>
#include <pimento.h>

#define INODE_IFDIR 0040000 // Directory
#define INODE_IFREG 0100000 // Regular file

struct vfsmount {
    struct dentry * mountpoint;
    struct superblock * superblock;
    struct dentry * root;
};

struct file_system {
    const char * name;
};

struct superblock {
    struct file_system * file_system;
    struct dentry * root;
    struct superblock_operations * operations;
};

struct dentry {
    struct dentry * parent;
    char * name;
    struct list * children;
    struct inode * inode;
    struct superblock * superblock;
    struct dentry_operations * operations;
};

struct inode {
    struct list * dentries;
    unsigned mode;
    loff_t size;
    struct superblock * superblock;
    struct inode_operations * operations;
    struct file_operations * file_operations;
    void * private_data;
};

struct file {
    struct inode * inode;
    unsigned pos;
    struct file_operations * operations;
};

struct superblock_operations {
};

struct dentry_operations {
};

struct inode_operations {
    int (* create)(struct inode *, struct dentry *, int);
    int (* mkdir)(struct inode *, struct dentry *, int);
};

struct file_operations {
    int (* open)(struct inode *, struct file *);
    ssize_t (* read)(struct file *, char *, size_t, loff_t *);
    ssize_t (* write)(struct file *, const char *, size_t, loff_t *);
};

struct path_token {
    const char * token;
    unsigned length;
};

struct path {
    struct dentry * parent;
    struct dentry * child;
    struct path_token last;
};

void vfs_init(void);
struct dentry * vfs_dentry_create(void);
void vfs_dentry_destroy(struct dentry *);
struct file * vfs_file_create(void);
void vfs_file_destroy(struct file *);
void vfs_mount(struct superblock *, struct dentry *);
int vfs_mkdir(struct path *, int);
int vfs_mknod(struct path *, int);
int vfs_open(struct path *, struct file *);
struct path * vfs_path_create(void);
void vfs_path_destroy(struct path *);
ssize_t vfs_read(struct file *, char *, size_t, loff_t *);
void vfs_resolve_path(struct path *, struct dentry *, const char *);
struct dentry * vfs_root(void);
ssize_t vfs_write(struct file *, const char *, size_t, loff_t *);
