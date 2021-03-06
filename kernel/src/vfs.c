#include "critical.h"
#include "list.h"
#include "pimento.h"
#include "vfs.h"

static struct list * _file_systems = 0;
static struct dentry * _dentry_root = 0;
static struct list * _mounts = 0;

/**
 * Create a virtual file system.
 */
void vfs_init(void)
{
    _file_systems = list_create();

    _dentry_root = vfs_dentry_create();
    _dentry_root->name = kmalloc(2);
    strncpy(_dentry_root->name, "/", 2);

    _mounts = list_create();
}

/**
 * Create a dentry.
 */
struct dentry * vfs_dentry_create(void)
{
    struct dentry * dentry = kcalloc(sizeof(struct dentry));
    dentry->children = list_create();

    return dentry;
}

/**
 * Destroy a dentry.
 */
void vfs_dentry_destroy(struct dentry * dentry)
{
    list_destroy(dentry->children);
    kfree(dentry);
}

/**
 * Create a file.
 */
struct file * vfs_file_create(void)
{
    struct file * file = kcalloc(sizeof(struct file));
    file->references = 0;

    return file;
}

/**
 * Destroy a file.
 */
void vfs_file_destroy(struct file * file)
{
    kfree(file);
}

/**
 * Create a mount point.
 */
int vfs_mount(struct path * path, struct superblock * superblock)
{
    if (path->child == 0) {
        return -ENOENT;
    }

    struct vfsmount * vfsmount = kcalloc(sizeof(struct vfsmount));
    vfsmount->mountpoint = path->child;
    vfsmount->superblock = superblock;
    vfsmount->root = superblock->root;

    critical_start();

    list_push_back(_mounts, vfsmount);

    critical_end();

    return 0;
}

/**
 * Map a dentry to a mount point, if one exists.
 */
static struct dentry * _map_mount(struct dentry * dentry)
{
    list_foreach(_mounts, struct vfsmount *, vfsmount) {
        if (vfsmount->mountpoint == dentry) {
            return vfsmount->root;
        }
    }

    return dentry;
}

/**
 * Create a directory.
 */
int vfs_mkdir(struct path * path, int mode)
{
    if (path->parent == 0) {
        return -ENOENT;
    }

    if ((path->parent->inode->mode & INODE_IFDIR) == 0) {
        return -ENOTDIR;
    }

    if (path->child != 0) {
        return -EPERM;
    }

    struct dentry * d_child = vfs_dentry_create();
    d_child->name = kmalloc(path->last.length);
    strncpy(d_child->name, path->last.token, path->last.length);

    struct inode * i_parent = path->parent->inode;
    i_parent->operations->mkdir(i_parent, d_child, mode);

    path->child = d_child;

    return 0;
}

/**
 * Create a node.
 */
int vfs_mknod(struct path * path, int mode)
{
    if (path->parent == 0) {
        return -ENOENT;
    }

    if ((path->parent->inode->mode & INODE_IFDIR) == 0) {
        return -ENOTDIR;
    }

    if (path->child != 0) {
        return -EPERM;
    }

    struct dentry * d_child = vfs_dentry_create();
    d_child->name = kmalloc(path->last.length);
    strncpy(d_child->name, path->last.token, path->last.length);

    struct inode * i_parent = path->parent->inode;
    i_parent->operations->create(i_parent, d_child, mode);

    path->child = d_child;

    return 0;
}

/**
 * Open a file.
 */
int vfs_open(struct path * path, struct file * file)
{
    if (path->child == 0) {
        return -ENOENT;
    }

    struct inode * i_child = path->child->inode;
    file->inode = i_child;
    file->operations = i_child->file_operations;
    i_child->file_operations->open(i_child, file);

    return 0;
}

/**
 * Create a path.
 */
struct path * vfs_path_create(void)
{
    return kcalloc(sizeof(struct path));
}

/**
 * Destroy a path.
 */
void vfs_path_destroy(struct path * path)
{
    kfree(path);
}

/**
 * Read from a file.
 */
ssize_t vfs_read(struct file * file, char * buf, size_t num, off_t * off)
{
    if (file->inode == 0) {
        return -ENOENT;
    }

    return file->operations->read(file, buf, num, off);
}

/**
 * Create a path token list from the given path. This function will eagerly
 * ignore multiple slashes.
 */
static struct list * _path_token_list(const char * path)
{
    struct list * path_tokens = list_create();

    while (*path == '/') {
        ++path;
    }

    while (*path != 0) {
        const char * end = path;
        while (*end != 0 && *end != '/') {
            ++end;
        }

        struct path_token * path_token = kcalloc(sizeof(struct path_token));
        path_token->token = path;
        path_token->length = end - path;
        list_push_back(path_tokens, path_token);

        path = end;

        while (*path == '/') {
            ++path;
        }
    }

    return path_tokens;
}

/**
 * Find a child dentry given its parent.
 */
static struct dentry * _dentry_find_child(struct dentry * d_parent,
                                          struct path_token * path_token)
{
    struct dentry * d_found = 0;

    if (strncmp(".", path_token->token, path_token->length) == 0) {
        d_found = d_parent;
    } else if (strncmp("..", path_token->token, path_token->length) == 0) {
        d_found = d_parent->parent;
    } else {
        list_foreach(d_parent->children, struct dentry *, d_child) {
            if (strncmp(d_child->name, path_token->token, path_token->length) == 0) {
                d_found = d_child;
                break;
            }
        }
    }

    return _map_mount(d_found);
}

/**
 * Resolve a path struct from a pathname, starting at the given dentry.
 */
void vfs_resolve_path(struct path * path, struct dentry * d_parent,
                      const char * pathname)
{
    struct dentry * d_child = 0;
    struct list * path_tokens = _path_token_list(pathname);
    int remaining = list_count(path_tokens) - 1;

    // Path is absolute.
    if (pathname[0] == '/') {
        d_parent = vfs_root();
    }

    list_foreach(path_tokens, struct path_token *, path_token)
    {
        d_child = _dentry_find_child(d_parent, path_token);

        // A dirname token was not found.
        if (remaining > 0 && d_child == 0) {
            break;
        }

        // A dirname token was found.
        if (remaining > 0 && d_child != 0) {
            d_parent = d_child;
            --remaining;
            continue;
        }

        path->parent = d_parent;

        // The basename token was not found.
        if (d_child == 0) {
            break;
        }

        path->child = d_child;
    }

    // Special case for the root directory.
    if (list_count(path_tokens) == 0) {
        path->parent = d_parent;
        path->child = d_parent;
    }

    // Record the last token in the pathname.
    struct path_token * last = list_peek_back(path_tokens);
    if (last != 0) {
        path->last.token = last->token;
        path->last.length = last->length;
    }

    list_destroy(path_tokens);
}

/**
 * Get the root node dentry.
 */
struct dentry * vfs_root(void)
{
    return _map_mount(_dentry_root);
}

/**
 * Write to a file.
 */
ssize_t vfs_write(struct file * file, const char * buf, size_t num,
                  off_t * off)
{
    if (file->inode == 0) {
        return -ENOENT;
    }

    return file->operations->write(file, buf, num, off);
}
