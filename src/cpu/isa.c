/**
 * isa.c: ISA lookup table and encode/decode helpers (see isa.h).
 * 
 * 2026-11-10 12:30 - isa - Jehu Zepeda-Silva - APIs for decoding/encoding
 * 2026-11-17 - Sriharsha Edukulla - initial HALT/NOP validation and disassembly (#26)
 * 2026-11-20 - Sriharsha Edukulla - add ADD, POP and LOAD disassembly after review
 */
#include "isa.h"

#include <inttypes.h>
#include <stdio.h>

bool isa_is_valid(uint8_t opcode)
{
    switch (opcode) {
        case OP_HALT:
        case OP_NOP:
        case OP_ADD:
        case OP_POP:
        case OP_LOAD:
            return true;
        default:
            return false;
    }
}

isa_instruction isa_decode(uint32_t word) 
{

}

uint32_t isa_encode(isa_instruction inst) 
{

}

const char* isa_mnemonic(uint8_t opcode) 
{

}

void isa_disassemble(uint32_t word, char* buf, size_t buflen)
{
    if (buf == NULL || buflen == 0) {
        return;
    }

    uint8_t opcode = (word >> ISA_OPCODE_SHIFT) & ISA_OPCODE_MASK;
    unsigned int rd = (word >> ISA_RD_SHIFT) & ISA_REG_MASK;
    unsigned int rs1 = (word >> ISA_RS1_SHIFT) & ISA_REG_MASK;
    unsigned int rs2 = (word >> ISA_RS2_SHIFT) & ISA_REG_MASK;
    uint32_t offset = word & ISA_IMM_MASK;

    switch (opcode) {
        case OP_HALT:
            snprintf(buf, buflen, "HALT");
            break;
        case OP_NOP:
            snprintf(buf, buflen, "NOP");
            break;
        case OP_ADD:
            snprintf(buf, buflen, "ADD R%u, R%u, R%u", rd, rs1, rs2);
            break;
        case OP_POP:
            // POP uses only rd; the other register fields are unused.
            snprintf(buf, buflen, "POP R%u", rd);
            break;
        case OP_LOAD:
            // LOAD's offset takes all 16 low bits, including the R-type rs2 field.
            snprintf(buf, buflen, "LOAD R%u, %" PRIu32 "(R%u)", rd, offset, rs1);
            break;
        default:
            // Keep the word visible while support for other instructions is pending.
            snprintf(buf, buflen, "UNSUPPORTED 0x%08" PRIX32, word);
            break;
    }
}
