#include "fs/ramfs.h"
#include "initrd.h"
#include "pimento.h"
#include "vfs.h"
#include "vfs_context.h"

extern const char _binary_build_initrd_tar_start;

/**
 * Given a ustar header, finds its content.
 */
inline const char * _content(struct ustar_header * header)
{
    const char * cheader = (const char *) header;

    return cheader + TAR_SECTOR_SIZE;
}

/**
 * Given a ustar header, find the next header.
 */
inline struct ustar_header * _next_header(struct ustar_header * header)
{
    uintptr_t next = TAR_SECTOR_SIZE + strtoul(header->size, 0, 8);
    next = ((next + TAR_SECTOR_SIZE - 1) / TAR_SECTOR_SIZE) * TAR_SECTOR_SIZE;

    return (struct ustar_header *) ((uintptr_t) header + next);
}

/**
 * Copy all contents from the ustar into the file system.
 */
static void _copy_tar_contents(struct ustar_header * header)
{
    while (1) {
        if (header->name[0] == 0) {
            break;
        }

        if (header->typeflag == TAR_TYPEFLAG_FILE) {
            struct path * path = vfs_path_create();
            struct file * file = vfs_file_create();

            vfs_resolve_path(path, vfs_root(), header->name);

            int res = vfs_mknod(path, 0644);
            if (res < 0) {
                vfs_path_destroy(path);
                vfs_file_destroy(file);

                break;
            }

            res = vfs_open(path, file);
            if (res < 0) {
                vfs_path_destroy(path);
                vfs_file_destroy(file);

                break;
            }

            const char * content = _content(header);
            off_t off = 0;
            vfs_write(file, content, strtoul(header->size, 0, 8), &off);

            vfs_path_destroy(path);
            vfs_file_destroy(file);
        } else if (header->typeflag == TAR_TYPEFLAG_DIR) {
            struct path * path = vfs_path_create();

            vfs_resolve_path(path, vfs_root(), header->name);

            int res = vfs_mkdir(path, 0755);
            if (res < 0) {
                vfs_path_destroy(path);

                break;
            }

            vfs_path_destroy(path);
        }

        header = _next_header(header);
    }
}

/**
 * Create a root filesystem, stored in RAM, that holds the contents of the
 * initrd tarball.
 */
void initrd_init(struct vfs_context * vfs_context)
{
    struct superblock * ramfs = ramfs_create();
    struct path * path = vfs_path_create();

    vfs_resolve_path(path, vfs_root(), "/");

    vfs_mount(path, ramfs);

    vfs_path_destroy(path);

    _copy_tar_contents((struct ustar_header *) &_binary_build_initrd_tar_start);

    vfs_context->pwd = vfs_root();
}
