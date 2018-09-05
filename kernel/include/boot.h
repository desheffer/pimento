#pragma once

#define MPIDR_AFF0_MASK (0b11111111UL << 0) // Affinity level 0

#define HCR_RW (0b1UL << 31) // Enable AArch64

#define CPACR_CP10 (0b11 << 20) // Full access to FP and SIMD

#define SCTLR_M (0b1 << 0) // Enable MMU
