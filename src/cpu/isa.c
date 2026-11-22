/**
 * isa.c: ISA lookup table and encode/decode helpers (see isa.h).
 *
 * 2026-11-10 12:30 - isa - Jehu Zepeda-Silva - APIs for decoding/encoding
 * 2026-11-17 - Sriharsha Edukulla - initial HALT/NOP validation and disassembly (#26)
 * 2026-11-20 - Sriharsha Edukulla - add ADD, POP and LOAD disassembly after review
 * 2026-11-22 - Chris Carl - added OPCODES, isa_instruction_constructor, isa_decode is a pointer, isa_disassemble_2, and print_isa_table
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"  // BUFFER
#include "isa.h"

const isa_opcode_info ISA_TABLE[OPCODES] = {
    [0x00] = {"HALT", FMT_NONE, FLAG_NONE, false, true},
    [0x01] = {"NOP", FMT_NONE, FLAG_NONE, false, true},
    [0x21] = {"POP", FMT_R, FLAG_NONE, false, true},
    [0x40] = {"LOAD", FMT_I, FLAG_NONE, false, true},
    [0x71] = {"ADD", FMT_R, (FLAG_Z | FLAG_O | FLAG_C), false, true},
};

bool isa_is_valid(uint8_t opcode) {
    switch (opcode) {
        // none
        case OP_HALT:
        case OP_NOP:
        case OP_RET:
        // stack
        case OP_PUSH:
        case OP_POP:
        // jumps
        case OP_JUMP:
        case OP_JR :
        case OP_CALL:
        // memory
        case OP_LOAD:
        case OP_STORE:
        case OP_MOV:
        case OP_LI:
        case OP_LUI:
        // unary
        case OP_NOT:
        case OP_NEG:
        // arithmetic
        case OP_CMP:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_ADDI:
        // logic
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_SL:
        case OP_SR:
        case OP_SLA:
        case OP_SRA:
        // branch
        case OP_BEQ:
        case OP_BNQ:
        case OP_BLT:
        case OP_BGT:
        case OP_BLE:
        case OP_BGE:
            return true;
        default:
            return false;
    }
}

isa_instruction* isa_instruction_constructor() {
    isa_instruction* isa_i = malloc(sizeof(isa_instruction));
    isa_i->op = 0;
    isa_i->rd = 0;
    isa_i->rs1 = 0;
    isa_i->rs2 = 0;
    isa_i->imm = 0;
    isa_i->addr = 0;
    return isa_i;
}

isa_instruction* isa_decode(uint32_t word) {
    uint8_t opcode = (word >> ISA_OPCODE_SHIFT) & ISA_OPCODE_MASK;
    if (!isa_is_valid(opcode)) return NULL;

    isa_opcode_info ici = ISA_TABLE[opcode];
    if (!ici.valid) fprintf(stderr, "WARNING: ISA_TABLE[0x%x] not valid!\n", opcode);

    unsigned int rd = (word >> ISA_RD_SHIFT) & ISA_REG_MASK;
    unsigned int rs1 = (word >> ISA_RS1_SHIFT) & ISA_REG_MASK;
    unsigned int rs2 = (word >> ISA_RS2_SHIFT) & ISA_REG_MASK;
    uint32_t imm = word & ISA_IMM_MASK;
    uint32_t addr = word & ISA_ADDR_MASK;

    isa_instruction* isa_i = isa_instruction_constructor();
    isa_i->op = opcode;

    switch (ici.fmt) {
        // case (FMT_NONE): {
        //     // nothing to add
        //     break;
        // }
        case (FMT_R): {
            // uses all 3 registers (even unary, binary)
            isa_i->rd = rd;
            isa_i->rs1 = rs1;
            isa_i->rs2 = rs2;
            break;
        }
        case (FMT_I): {
            // uses 2 registers and rest are immediate
            isa_i->rd = rd;
            isa_i->rs1 = rs1;
            isa_i->imm = imm;
            break;
        }
        case (FMT_J): {
            // uses 0 registers
            isa_i->addr = addr;
            break;
        }
        default: {
            break;  // not possible
        }
    }

    return isa_i;
}

uint32_t isa_encode(isa_instruction inst) {
    isa_opcode_info info = ISA_TABLE[inst.op];

    uint32_t binary = 0;
    if (info.fmt == FMT_R) {
        binary = inst.op << ISA_OPCODE_SHIFT | inst.rd << ISA_RD_SHIFT | inst.rs1 << ISA_RS1_SHIFT | inst.rs2 << ISA_RS2_SHIFT; // 0-11 unused
    } else if (info.fmt == FMT_I) {
        binary = inst.op << ISA_OPCODE_SHIFT | inst.rd << ISA_RD_SHIFT | inst.rs1 << ISA_RS1_SHIFT | inst.imm;
    } else if (info.fmt == FMT_J) {
        binary = inst.op << ISA_OPCODE_SHIFT | inst.addr;
    } else {
        binary = inst.op << ISA_OPCODE_SHIFT;
    }

    return binary;
}

const char* isa_mnemonic(uint8_t opcode) {
    switch (opcode) {
        case (OP_HALT): return "HALT";
        case (OP_NOP): return "NOP";
        case (OP_RET): return "RET";
        case (OP_PUSH): return "PUSH";
        case (OP_POP): return "POP";
        case (OP_JUMP): return "JUMP";
        case (OP_JR ): return "JR ";
        case (OP_CALL): return "CALL";
        case (OP_LOAD): return "LOAD";
        case (OP_STORE): return "STORE";
        case (OP_MOV): return "MOV";
        case (OP_LI): return "LI";
        case (OP_LUI): return "LUI";
        case (OP_NOT): return "NOT";
        case (OP_NEG): return "NEG";
        case (OP_CMP): return "CMP";
        case (OP_ADD): return "ADD";
        case (OP_SUB): return "SUB";
        case (OP_MUL): return "MUL";
        case (OP_DIV): return "DIV";
        case (OP_ADDI): return "ADDI";
        case (OP_AND): return "AND";
        case (OP_OR): return "OR";
        case (OP_XOR): return "XOR";
        case (OP_SL): return "SL";
        case (OP_SR): return "SR";
        case (OP_SLA): return "SLA";
        case (OP_SRA): return "SRA";
        case (OP_BEQ): return "BEQ";
        case (OP_BNQ): return "BNQ";
        case (OP_BLT): return "BLT";
        case (OP_BGT): return "BGT";
        case (OP_BLE): return "BLE";
        case (OP_BGE): return "BGE";
        default: return "UNSUPPORTED";
    }
}

const char* isa_operands(uint8_t opcode, uint8_t instruction_format) {
    switch (instruction_format) {
        case (FMT_NONE): return "";
        case (FMT_R): {
            uint8_t OPS = ISA_OP_OPS(opcode);
            if (OPS == 1) return "%u";
            if (OPS == 2) return "%u %u";
            return "%u %u %u";
        }
        case (FMT_I): {
            if (opcode == OP_LOAD) return "%u %u(%u)";
            if (opcode == OP_STORE) return "%u(%u) %u";
            return "%u %u %u";
        }
        case (FMT_J): return "%x";
        default: return "?";
    }
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

void isa_disassemble_2(uint32_t word, char* buf, size_t buflen) {
    isa_instruction* isa_i = isa_decode(word);
    isa_opcode_info info = ISA_TABLE[isa_i->op];
    const char* mnemonic = isa_mnemonic(isa_i->op);
    const char* operands = isa_operands(isa_i->op, info.fmt);
    if (info.fmt == FMT_NONE) {
        sprintf(BUFFER, "%s", mnemonic);
    } else {
        sprintf(BUFFER, "%s %s", mnemonic, operands);
        switch (info.fmt) {
            case (FMT_R): {
                sprintf(BUFFER, BUFFER, isa_i->rd, isa_i->rs1, isa_i->rs2);
                break;
            }
            case (FMT_I): {
                if (isa_i->op == OP_LOAD)
                    sprintf(BUFFER, BUFFER, isa_i->rd, isa_i->imm, isa_i->rs1);
                else if (isa_i->op == OP_STORE)
                    sprintf(BUFFER, BUFFER, isa_i->imm, isa_i->rd, isa_i->rs1);
                else
                    sprintf(BUFFER, BUFFER, isa_i->rd, isa_i->rs1, isa_i->imm);
                break;
            }
            case (FMT_J): {
                sprintf(BUFFER, BUFFER, isa_i->addr);
                break;
            }
            default: break;  // not possible
        }
    }
    strncpy(buf, BUFFER, buflen - 1);
}

void print_isa_table() {
    printf("|Instr  |Syntax              |Type|jmp|ops|alu|ins|Opcode|Flags|\n");
    printf("|---    |---                 |--- |---|---|---|---|---   |---  |\n");
    for (uint16_t opcode = 0; opcode < OPCODES; opcode++) {
        isa_opcode_info info = ISA_TABLE[opcode];
        if (!info.valid) continue;
        uint8_t jmp = ISA_OP_JMP(opcode);
        uint8_t ops = ISA_OP_OPS(opcode);
        uint8_t alu = ISA_OP_ALU(opcode);
        uint8_t ins = ISA_OP_INS(opcode);
        const char* mnemonic = isa_mnemonic(opcode);
        const char* operands = isa_operands(opcode, info.fmt);
        char type = 'N';
        switch (info.fmt) {
            case (FMT_R): type = 'R'; break;
            case (FMT_I): type = 'I'; break;
            case (FMT_J): type = 'J'; break;
            default: type = 'N'; break;
        }
        char Z = info.flags_set && FLAG_Z ? 'Z' : '-';
        char O = info.flags_set && FLAG_O ? 'O' : ' ';
        char C = info.flags_set && FLAG_C ? 'C' : ' ';
        printf(
            "|%s   |%s %s   |%c   |%d  |%d  |%d  |%d  |0x%x|%c %c %c|\n",
            mnemonic, mnemonic, operands, type, jmp, ops, alu, ins, opcode, Z, O, C);
    }

}
