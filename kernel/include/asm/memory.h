#pragma once

#define PAGE_SIZE 4096

#define VA_BITS  48
#define VA_START (0xFFFFFFFFFFFFFFFF - ((0x1UL << VA_BITS) - 1))

#define MT_DEVICE_nGnRnE 0
#define MT_NORMAL        1
#define MT_NORMAL_NC     2

#define MAIR_EL1 \
    ( \
        (0x00UL << (8 * MT_DEVICE_nGnRnE)) | \
        (0xFFUL << (8 * MT_NORMAL)) | \
        (0x44UL << (8 * MT_NORMAL_NC)) \
    )

#define TCR_ASID16     (0b1UL  << 36) // Use 16 bits of ASID
#define TCR_TG1_4KB    (0b10UL << 30) // 4KB granule size
#define TCR_TG0_4KB    (0b00UL << 14) // 4KB granule size
#define TCR_TxSZ(bits) (((64 - bits) << 16) | ((64 - bits) << 0))

#define TCR_EL1 \
    ( \
        TCR_TxSZ(VA_BITS) | \
        TCR_ASID16 | TCR_TG1_4KB | TCR_TG0_4KB \
    )
