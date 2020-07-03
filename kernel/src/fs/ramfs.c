#include <byte_array.h>
#include <fs/ramfs.h>
#include <list.h>
#include <page.h>
#include <pimento.h>
#include <vfs.h>

static struct file_system _file_system = {
    .name = "ramfs",
};

static struct superblock_operations _superblock_operations = {
};

static struct dentry_operations _dentry_operations = {
};

static struct inode_operations _inode_operations = {
    .create = ramfs_inode_create,
    .mkdir = ramfs_inode_mkdir,
};

static struct file_operations _file_operations = {
    .open = ramfs_file_open,
    .read = ramfs_file_read,
    .write = ramfs_file_write,
};

/**
 * Create a RAM file system.
 */
struct superblock * ramfs_create(void)
{
    struct inode * i_root = kcalloc(sizeof(struct inode));
    struct dentry * d_root = vfs_dentry_create();
    struct superblock * superblock = kcalloc(sizeof(struct superblock));

    superblock->file_system = &_file_system;
    superblock->root = d_root;
    superblock->operations = &_superblock_operations;

    d_root->name = kmalloc(2);
    strncpy(d_root->name, "/", 2);
    d_root->inode = i_root;
    d_root->superblock = superblock;
    d_root->operations = &_dentry_operations;

    i_root->dentries = list_create();
    list_push_back(i_root->dentries, d_root);
    i_root->mode = 0755 | INODE_IFDIR;
    i_root->superblock = superblock;
    i_root->operations = &_inode_operations;
    i_root->file_operations = &_file_operations;
    i_root->private_data = byte_array_create();

    return superblock;
}

/**
 * Open a file.
 */
int ramfs_file_open(struct inode * inode, struct file * file)
{
    (void) inode;
    (void) file;

    return 0;
}

/**
 * Read from a file.
 */
ssize_t ramfs_file_read(struct file * file, char * buf, size_t num,
                        loff_t * off)
{
    num = byte_array_copy(file->inode->private_data, buf, num, *off);

    *off += num;
    file->pos = *off;

    return num;
}

/**
 * Write to a file.
 */
ssize_t ramfs_file_write(struct file * file, const char * buf, size_t num,
                         loff_t * off)
{
    if (file->inode->size > 0) {
        // @TODO: Erase bytes starting at index `off`
        return -999;
    }

    num = byte_array_append(file->inode->private_data, buf, num);

    *off += num;
    file->inode->size = *off;

    return num;
}

/**
 * Create an inode.
 */
int ramfs_inode_create(struct inode * dir, struct dentry * d_child, int mode)
{
    int res = -ENOENT;

    list_foreach(dir->dentries, struct dentry *, d_parent) {
        struct inode * i_child = kcalloc(sizeof(struct inode));

        d_child->parent = d_parent;
        d_child->inode = i_child;
        d_child->superblock = dir->superblock;
        d_child->operations = &_dentry_operations;

        i_child->dentries = list_create();
        list_push_back(i_child->dentries, d_child);
        i_child->mode = (mode & 0777) | INODE_IFREG;
        i_child->superblock = dir->superblock;
        i_child->operations = &_inode_operations;
        i_child->file_operations = &_file_operations;
        i_child->private_data = byte_array_create();

        list_push_back(d_parent->children, d_child);

        res = 0;

        break;
    }

    return res;
}

/**
 * Create a directory inode.
 */
int ramfs_inode_mkdir(struct inode * dir, struct dentry * d_child, int mode)
{
    int res = -ENOENT;

    list_foreach(dir->dentries, struct dentry *, d_parent) {
        struct inode * i_child = kcalloc(sizeof(struct inode));

        d_child->parent = d_parent;
        d_child->inode = i_child;
        d_child->superblock = dir->superblock;
        d_child->operations = &_dentry_operations;

        i_child->dentries = list_create();
        list_push_back(i_child->dentries, d_child);
        i_child->mode = (mode & 0777) | INODE_IFDIR;
        i_child->superblock = dir->superblock;
        i_child->operations = &_inode_operations;
        i_child->file_operations = &_file_operations;

        list_push_back(d_parent->children, d_child);

        res = 0;

        break;
    }

    return res;
}
