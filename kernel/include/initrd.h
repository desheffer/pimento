#pragma once

#define TAR_SECTOR_SIZE 512

#define TAR_TYPEFLAG_FILE '0'
#define TAR_TYPEFLAG_DIR  '5'

#include "pimento.h"
#include "vfs_context.h"

struct ustar_header {
    const char name[100];
    const char mode[8];
    const char uid[8];
    const char gid[8];
    const char size[12];
    const char mtime[12];
    const char chksum[8];
    const char typeflag;
    const char linkname[100];
    const char magic[6];
    const char version[2];
    const char uname[32];
    const char gname[32];
    const char devmajor[8];
    const char devminor[8];
    const char prefix[155];
} __attribute__((packed));

void initrd_init(struct vfs_context *);
