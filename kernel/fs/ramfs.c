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
    superblock->type = ramfs;

    struct dentry * dentry_root = kzalloc(sizeof(struct dentry));
    dentry_root->name = "";
    dentry_root->superblock = superblock;
    dentry_root->dentry_children = list_create();

    // @TODO: The following creates fake entries.
    struct dentry * dentry_root_root = kzalloc(sizeof(struct dentry));
    dentry_root_root->name = "root";
    dentry_root_root->dentry_parent = dentry_root;
    dentry_root_root->superblock = superblock;
    dentry_root_root->dentry_children = list_create();

    struct dentry * dentry_root_bin = kzalloc(sizeof(struct dentry));
    dentry_root_bin->name = "bin";
    dentry_root_bin->dentry_parent = dentry_root;
    dentry_root_bin->superblock = superblock;
    dentry_root_bin->dentry_children = list_create();

    superblock->dentry_root = dentry_root;

    list_push_back(dentry_root->dentry_children, dentry_root_root);
    list_push_back(dentry_root->dentry_children, dentry_root_bin);

    list_push_back(_dentry_roots, superblock->dentry_root);

    return dentry_root;
}
