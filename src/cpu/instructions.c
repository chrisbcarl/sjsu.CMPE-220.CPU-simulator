/*
2026-11-22 - Chris Carl - added ADD, LI, LUI
*/
#include <stdint.h>

#include "instructions.h"
#include "isa.h"
#include "core.h"

void LI(uint32_t* rd, uint16_t* imm) {
    // 2026-11-22 - Chris Carl
    *rd = *imm;
}

void LUI(uint32_t* rd, uint16_t* imm) {
    // 2026-11-22 - Chris Carl
    *rd = *imm << 16;
}

void ADD(uint32_t* rd, uint32_t* rs1, uint32_t* rs2, uint8_t* flags) {
    // 2026-11-22 - Chris Carl
    *rd = *rs1 + *rs2 + FLAG_C_OF(*flags);
    if (*rd == 0) *flags |= FLAG_Z;
    if (*rs2 > 0xFFFFFFFF - *rs1) *flags |= FLAG_C;  // in hardware this is thanks to full adder C
}
