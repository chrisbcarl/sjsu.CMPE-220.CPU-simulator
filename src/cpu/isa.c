/**
 * isa.c: ISA lookup table and encode/decode helpers (see isa.h).
 * 
 * 2026-11-10 12:30 - isa - Jehu Zepeda-Silva - APIs for decoding/encoding
 */
#include "isa.h"

bool isa_is_valid(uint8_t opcode) 
{

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

}