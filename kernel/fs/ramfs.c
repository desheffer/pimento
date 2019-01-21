#include <kstdlib.h>
#include <list.h>
#include <ramfs.h>

static struct list * _dentry_roots = 0;

struct dentry * ramfs_create(void)
{
    if (_dentry_roots == 0) {
        _dentry_roots = list_create();
    }

    struct superblock * superblock = kzalloc(sizeof(struct superblock));
    superblock->type = file_system_type_ramfs;

    // Directory: /
    struct inode * inode_root = kzalloc(sizeof(struct inode));
    inode_root->type = inode_type_directory;
    inode_root->superblock = superblock;

    struct dentry * dentry_root = kzalloc(sizeof(struct dentry));
    dentry_root->inode = inode_root;
    dentry_root->name = "";
    dentry_root->superblock = superblock;
    dentry_root->dentry_children = list_create();

    superblock->dentry_root = dentry_root;
    list_push_back(_dentry_roots, superblock->dentry_root);

    // Directory: /root
    struct inode * inode_root_root = kzalloc(sizeof(struct inode));
    inode_root_root->type = inode_type_directory;
    inode_root_root->superblock = superblock;

    struct dentry * dentry_root_root = kzalloc(sizeof(struct dentry));
    dentry_root_root->inode = inode_root_root;
    dentry_root_root->name = "root";
    dentry_root_root->dentry_parent = dentry_root;
    dentry_root_root->superblock = superblock;
    dentry_root_root->dentry_children = list_create();

    list_push_back(dentry_root->dentry_children, dentry_root_root);

    // Directory: /bin
    struct inode * inode_root_bin = kzalloc(sizeof(struct inode));
    inode_root_bin->type = inode_type_directory;
    inode_root_bin->superblock = superblock;

    struct dentry * dentry_root_bin = kzalloc(sizeof(struct dentry));
    dentry_root_bin->inode = inode_root_bin;
    dentry_root_bin->name = "bin";
    dentry_root_bin->dentry_parent = dentry_root;
    dentry_root_bin->superblock = superblock;
    dentry_root_bin->dentry_children = list_create();

    list_push_back(dentry_root->dentry_children, dentry_root_bin);

    // File: /bin/sh
    struct inode * inode_root_bin_sh = kzalloc(sizeof(struct inode));
    inode_root_bin_sh->type = inode_type_file;
    inode_root_bin_sh->superblock = superblock;

    struct dentry * dentry_root_bin_sh = kzalloc(sizeof(struct dentry));
    dentry_root_bin_sh->inode = inode_root_bin_sh;
    dentry_root_bin_sh->name = "sh";
    dentry_root_bin_sh->dentry_parent = dentry_root_bin;
    dentry_root_bin_sh->superblock = superblock;

    list_push_back(dentry_root_bin->dentry_children, dentry_root_bin_sh);

    // Files: /bin/*
    const char * const utils[] = {
        "help",
        "toybox",
        0
    };

    struct inode * inode_root_bin_toybox = kzalloc(sizeof(struct inode));
    inode_root_bin_toybox->type = inode_type_file;
    inode_root_bin_toybox->superblock = superblock;

    for (const char * const * iter = utils; *iter != 0; ++iter) {
        struct dentry * dentry_root_bin_help = kzalloc(sizeof(struct dentry));
        dentry_root_bin_help->inode = inode_root_bin_toybox;
        dentry_root_bin_help->name = *iter;
        dentry_root_bin_help->dentry_parent = dentry_root_bin;
        dentry_root_bin_help->superblock = superblock;

        list_push_back(dentry_root_bin->dentry_children, dentry_root_bin_help);
    }

    return dentry_root;
}
