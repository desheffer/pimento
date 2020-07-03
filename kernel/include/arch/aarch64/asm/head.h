#pragma once

#define MPIDR_AFF0_MASK 0b11111111UL // Affinity level 0

#define HCR_RW (0b1UL << 31) // Enable AArch64

#define CPACR_CP10 (0b11 << 20) // Full access to FP and SIMD

#define SCTLR_M (0b1 << 0) // Enable MMU

#define PSR_MODE_M_EL0  (0b0000 << 0)
#define PSR_MODE_M_EL1t (0b0100 << 0)
#define PSR_MODE_M_EL1h (0b0101 << 0)

#define PSR_MODE_D (0b1 << 9) // Mask Sync
#define PSR_MODE_A (0b1 << 8) // Mask Error
#define PSR_MODE_I (0b1 << 7) // Mask IRQ
#define PSR_MODE_F (0b1 << 6) // Mask FIQ

#define PSR_MODE_INIT \
    (PSR_MODE_D | PSR_MODE_A | PSR_MODE_I | PSR_MODE_F | PSR_MODE_M_EL1h)
#define PSR_MODE_USER (PSR_MODE_M_EL0)

#ifndef __ASSEMBLER__

void hang(void);

#endif
