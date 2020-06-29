#include <byte_array.h>
#include <fs/devfs.h>
#include <fs/ramfs.h>
#include <list.h>
#include <page.h>
#include <pimento.h>
#include <vfs.h>

/**
 * Create a device file system based in RAM.
 */
void devfs_init(void)
{
    struct superblock * devfs = ramfs_create();
    struct path * path = vfs_path_create();

    vfs_resolve_path(path, 0, "/dev");

    vfs_mkdir(path, 0755);
    vfs_mount(path, devfs);

    vfs_path_destroy(path);
}
