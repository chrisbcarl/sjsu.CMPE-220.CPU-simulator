/*
2026-11-22 - Chris Carl - instructions.c - added ADD, LI, LUI
2026-11-23 - Chris Carl - instructions.c - ADD overflow conditions, LUI keeps lower in tact
*/
#include <stdint.h>

#include "instructions.h"
#include "isa.h"
#include "core.h"

// #include <stdio.h>

void _flags_deassert(uint8_t* flags) {
    *flags &= !FLAG_Z;
    *flags &= !FLAG_C;
    *flags &= !FLAG_O;
}

void _flags_assert(uint32_t* rd, uint32_t* rs1, uint32_t* rs2, uint8_t* flags) {
    if (*rd == 0) *flags |= FLAG_Z;

    // in unsigned, if rs2 is bigger than rs1 enough to go over 0xF...F, then carry happened
    // printf("*rs2 0x%x, 0xFFFFFFFF - *rs1 0x%x, condition %d\n", *rs2, 0xFFFFFFFF - *rs1, *rs2 > (0xFFFFFFFF - *rs1));
    if (*rs2 > (0xFFFFFFFF - *rs1)) *flags |= FLAG_C;  // in hardware this is handled instantly due to full adder's C output

    // in signed, two negs equals neg, two pos equals pos, opposite can't overflow
    // 1 + 1 = 1, 0 + 0 = 0, 0 + 1 = X, anything else is bad....
    bool rs1_sign = (*rs1 >> 31) & 1;
    bool rs2_sign = (*rs2 >> 31) & 1;
    bool rd_sign = (*rd >> 31) & 1;
    // printf("rs1_sign %d | rs2_sign %d | rd_sign %d\n", rs1_sign, rs2_sign, rd_sign);
    if (rs1_sign == rs2_sign) {
        if (rd_sign != rs1_sign)  *flags |= FLAG_O;
    }
}

void LI(uint32_t* rd, uint16_t* imm) {
    // 2026-11-22 - Chris Carl
    *rd = *imm;
}

void LUI(uint32_t* rd, uint16_t* imm) {
    // 2026-11-22 - Chris Carl
    *rd |= *imm << 16;
}

void ADD(uint32_t* rd, uint32_t* rs1, uint32_t* rs2, uint8_t* flags) {
    // 2026-11-22 - Chris Carl
    _flags_deassert(flags);

    *rd = *rs1 + *rs2;  // in general, do not re-use leftover carries. if we later decide to include them, add back + FLAG_C_OF(*flags)

    _flags_assert(rd, rs1, rs2, flags);
}
