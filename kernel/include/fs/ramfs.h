#pragma once

#include "pimento.h"
#include "vfs.h"

struct superblock * ramfs_create(void);
int ramfs_file_open(struct inode *, struct file *);
ssize_t ramfs_file_read(struct file *, char *, size_t, off_t *);
ssize_t ramfs_file_write(struct file *, const char *, size_t, off_t *);
int ramfs_inode_create(struct inode *, struct dentry *, int);
int ramfs_inode_mkdir(struct inode *, struct dentry *, int);
