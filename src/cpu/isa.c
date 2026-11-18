/**
 * isa.c: ISA lookup table and encode/decode helpers (see isa.h).
 * 
 * 2026-11-10 12:30 - isa - Jehu Zepeda-Silva - APIs for decoding/encoding
 * 2026-11-17 - Sriharsha Edukulla - initial HALT/NOP validation and disassembly (#26)
 */
#include "isa.h"

#include <inttypes.h>
#include <stdio.h>

bool isa_is_valid(uint8_t opcode)
{
    // Starting with HALT and NOP. The remaining ISA opcodes still need support.
    return opcode == OP_HALT || opcode == OP_NOP;
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
    if (!isa_is_valid(opcode)) {
        // Keep the word visible while support for the other instructions is pending.
        snprintf(buf, buflen, "UNSUPPORTED 0x%08" PRIX32, word);
        return;
    }

    // Neither instruction has operands, so the lower 24 bits are unused.
    snprintf(buf, buflen, "%s", opcode == OP_HALT ? "HALT" : "NOP");
}
