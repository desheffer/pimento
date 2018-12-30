#pragma once

#define PROCESS_REGS_SIZE (34 * 8)

#define ESR_ELx_EC_SHIFT 26
#define ESR_ELx_EC_MASK  (0b111111UL << ESR_ELx_EC_SHIFT)
#define ESR_ELx_EC(ecr)  (((ecr) & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)

#define ESR_ELx_EC_ILL      0b001110
#define ESR_ELx_EC_SVC64    0b010101
#define ESR_ELx_EC_IABT_LOW 0b100000
#define ESR_ELx_EC_IABT_CUR 0b100001
#define ESR_ELx_EC_PC_ALIGN 0b100010
#define ESR_ELx_EC_DABT_LOW 0b100100
#define ESR_ELx_EC_DABT_CUR 0b100101
#define ESR_ELx_EC_SP_ALIGN 0b100110
#define ESR_ELx_EC_BRK64    0b111100

#define ESR_ELx_xFSC_SHIFT 0
#define ESR_ELx_xFSC_MASK  (0b111111UL << ESR_ELx_xFSC_SHIFT)
#define ESR_ELx_xFSC(fsc)  (((fsc) & ESR_ELx_xFSC_MASK) >> ESR_ELx_xFSC_SHIFT)

#define ESR_ELx_xFSC_ASF_0 0b000000
#define ESR_ELx_xFSC_ASF_1 0b000001
#define ESR_ELx_xFSC_ASF_2 0b000010
#define ESR_ELx_xFSC_ASF_3 0b000011
#define ESR_ELx_xFSC_TF_0  0b000100
#define ESR_ELx_xFSC_TF_1  0b000101
#define ESR_ELx_xFSC_TF_2  0b000110
#define ESR_ELx_xFSC_TF_3  0b000111
#define ESR_ELx_xFSC_AFF_1 0b001001
#define ESR_ELx_xFSC_AFF_2 0b001010
#define ESR_ELx_xFSC_AFF_3 0b001011
#define ESR_ELx_xFSC_PF_1  0b001101
#define ESR_ELx_xFSC_PF_2  0b001110
#define ESR_ELx_xFSC_PF_3  0b001111

#define BAD_SYNC  0
#define BAD_IRQ   1
#define BAD_FIQ   2
#define BAD_ERROR 3

#ifndef __ASSEMBLY__

typedef struct registers_t {
    long unsigned regs[31]; // x0 - x30
    long unsigned sp;       // sp_el0
    long unsigned pc;       // elr_el1
    long unsigned pstate;   // spsr_el1
} registers_t;

#endif
