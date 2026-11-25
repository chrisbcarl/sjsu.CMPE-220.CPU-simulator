/**
 * isa.c: ISA lookup table and encode/decode helpers (see isa.h).
 *
 * 2026-11-10 12:30 - Jehu Zepeda-Silva - isa.c - APIs for decoding/encoding
 * 2026-11-17 - Sriharsha Edukulla - isa.c - initial HALT/NOP validation and disassembly (#26)
 * 2026-11-20 - Sriharsha Edukulla - isa.c - add ADD, POP and LOAD disassembly after review
 * 2026-11-22 - Chris Carl - isa.c - added OPCODES, isa_instruction_constructor, isa_decode is a pointer, isa_disassemble_2, and print_isa_table
 * 2026-11-23 - Chris Carl - isa.c - added (isa.c){LI/LUI in ISA_TABLE, isa_operands, isa_disassemble_2}
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"  // COMMON_BUFFER_0
#include "isa.h"

/**
 * Array of opcode_info meta description
 *  {
 *      "mnemonic string"
 *      NONE, R-type, I-type, J-type,
 *      flags affected, Z|O|C
 *      should immediate by sign extended?
 *      true - is this meta description valid? always true, because the holes of ISA_TABLE are not valid
 *  }
 */
const isa_opcode_info ISA_TABLE[OPCODES] = {
    [0x00] = {"HALT", FMT_NONE, FLAG_NONE, false, true},
    [0x01] = {"NOP", FMT_NONE, FLAG_NONE, false, true},
    [0x21] = {"POP", FMT_R, FLAG_NONE, false, true},
    [0x40] = {"LOAD", FMT_I, FLAG_NONE, false, true},
    [0x43] = {"LI", FMT_I, FLAG_NONE, false, true},
    [0x44] = {"LUI", FMT_I, FLAG_NONE, false, true},
    [0x71] = {"ADD", FMT_R, (FLAG_Z | FLAG_O | FLAG_C), false, true},
};

bool isa_is_valid(uint8_t opcode) {
    /**
     * Return whether an 8-bit number is a valid opcode within our ISA.
     *
     * @param opcode - uint8_t - 8 bit opcode
     * @return - bool - this 8-bit number is or isn't legit
     */

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
    /**
     * @authors Chris Carl
     * isa_instruction constructor/initializer
     * @return - isa_instruction* - NULL or the real deal
     */

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
    /**
     * @authors Chris Carl
     * Given a 32-bit number, get back an isa_instruction struct pointer
     *  ex) isa_decode(0x71312000) returns {op=0x71, rd=3, rs1=1, rs2=2, imm=0, addr=0}
     *  ex) isa_decode(0x4430DEAD) returns {op=0x74, rd=3, rs1=0, rs2=X, imm=0xDEAD, addr=0}
     *
     * @param word - uint32_t - 32-bit number or binary instruction
     * @return - isa_instruction* - NULL or the real deal
     */
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

uint32_t isa_encode(isa_instruction* inst) {
    /**
     * @authors Chris Carl
     * Given an isa_instruction struct pointer, get back a number for that binary instruction.
     *  ex) isa_decode({op=0x71, rd=3, rs1=1, rs2=2, imm=0, addr=0})      returns 0x71312000
     *  ex) isa_decode({op=0x74, rd=3, rs1=0, rs2=X, imm=0xDEAD, addr=0}) returns 0x4430DEAD
     *
     * @param inst - isa_instruction* - isa_instruction struct pointer
     * @return - uint32_t - number for that binary instruction
     */

    isa_opcode_info info = ISA_TABLE[inst->op];

    uint32_t binary = 0;
    if (info.fmt == FMT_R) {
        binary = inst->op << ISA_OPCODE_SHIFT | inst->rd << ISA_RD_SHIFT | inst->rs1 << ISA_RS1_SHIFT | inst->rs2 << ISA_RS2_SHIFT; // 0-11 unused
    } else if (info.fmt == FMT_I) {
        binary = inst->op << ISA_OPCODE_SHIFT | inst->rd << ISA_RD_SHIFT | inst->rs1 << ISA_RS1_SHIFT | inst->imm;
    } else if (info.fmt == FMT_J) {
        binary = inst->op << ISA_OPCODE_SHIFT | inst->addr;
    } else {
        binary = inst->op << ISA_OPCODE_SHIFT;
    }

    return binary;
}

const char* isa_mnemonic(uint8_t opcode) {
    /**
     * @authors Chris Carl
     * Given some 8-bit number, get back what our ISA says is its "mnemonic" or the "instruction class" in assembly
     *  ex) isa_mnemonic(0x00) returns "HALT"
     *  ex) isa_mnemonic(0x71) returns "ADD"
     * NOTE: typically you'll be using an isa_instruction struct to get a word's opcode and format enum
     *
     * @param opcode - uint8_t - 8-bit number, hopefully valid
     * @return - const char* - NULL or a string to print
     */
    if (!isa_is_valid(opcode)) return NULL;
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
    /**
     * @authors Chris Carl
     * Given this opcode, what should its operands look like?
     * You get back a formattable string of operands.
     *  ex) isa_operands(0x71, FMT_R) = "R%u, R%u, R%u"
     *  ex) isa_operands(0x44, FMT_I) = "R%u, %u"
     * NOTE: typically you'll be using an isa_instruction struct to get a word's opcode and format enum
     *
     * @param - uint8_t - opcode
     * @param - uint8_t - instruction_format (FMT_NONE, FMT_R, FMT_I, FMT_J)
     * @return - const char*
     */
    switch (instruction_format) {
        case (FMT_NONE): return "";
        case (FMT_R): {
            uint8_t OPS = ISA_OP_OPS(opcode);
            if (OPS == 1) return "R%u";
            if (OPS == 2) return "R%u, R%u";
            return "R%u, R%u, R%u";
        }
        case (FMT_I): {
            if (opcode == OP_LOAD) return "R%u, %u(R%u)";
            if (opcode == OP_STORE) return "%u(R%u), R%u";
            if (opcode == OP_LI) return "R%u, %u";
            if (opcode == OP_LUI) return "R%u, %u";
            return "R%u, R%u, 0x%x";  // the branchers
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

void isa_disassemble_2(uint32_t word, char* buffer, size_t buffer_size) {
    /**
     * @authors Chris Carl
     * Disassemble: number to ASM string
     * Given some binary instruction as a number, fill up a character buffer with what the ASM string is supposed to be
     *
     * @param word - uint32_t - the binary instruction as a number
     * @param buffer - char* - some character array you want to fill with the ASM instruction
     * @param buffer_size - size_t - how big is the char array you sent? bust be larger than ASM instruction
     */
    isa_instruction* isa_i = isa_decode(word);
    isa_opcode_info info = ISA_TABLE[isa_i->op];
    const char* mnemonic = isa_mnemonic(isa_i->op);
    const char* operands = isa_operands(isa_i->op, info.fmt);
    size_t i = 0; while (i < COMMON_BUFFER_LEN && COMMON_BUFFER_0[i] != '\0') {COMMON_BUFFER_0[i] = '\0'; i += 1;}
    i = 0; while (i < COMMON_BUFFER_LEN && COMMON_BUFFER_1[i] != '\0') {COMMON_BUFFER_1[i] = '\0'; i += 1;}

    if (info.fmt == FMT_NONE) {
        sprintf(COMMON_BUFFER_1, "%s", mnemonic);
    } else {
        // fill up COMMON_BUFFER_0 with a format string like "ADD R%u, R%u, R%u"
        sprintf(COMMON_BUFFER_0, "%s %s", mnemonic, operands);

        // fill up COMMON_BUFFER_1 with that format string formatted with actual values for rd, rs1, imm, addr, etc.
        switch (info.fmt) {
            case (FMT_R): {
                sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->rd, isa_i->rs1, isa_i->rs2);
                break;
            }
            case (FMT_I): {
                if (isa_i->op == OP_LOAD)
                    sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->rd, isa_i->imm, isa_i->rs1);
                else if (isa_i->op == OP_STORE)
                    sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->imm, isa_i->rd, isa_i->rs1);
                else if (isa_i->op == OP_LI)
                    sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->rd, isa_i->imm);
                else if (isa_i->op == OP_LUI)
                    sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->rd, isa_i->imm);
                else
                    // the branchers
                    sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->rd, isa_i->rs1, isa_i->imm);
                break;
            }
            case (FMT_J): {
                sprintf(COMMON_BUFFER_1, COMMON_BUFFER_0, isa_i->addr);
                break;
            }
            default: break;  // not possible
        }
    }
    // finally copy the formatted string COMMON_BUFFER_1 into the container the user needs
    strncpy(buffer, COMMON_BUFFER_1, buffer_size - 1);
}

void print_isa_table() {
    /**
     * @authors Chris Carl
     * We've written up isa.md. Can we reproduce it exactly? That's the goal here.
     */
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
        char C = info.flags_set && FLAG_C ? 'C' : ' ';
        char O = info.flags_set && FLAG_O ? 'O' : ' ';
        printf(
            "|%s   |%s %s   |%c   |%d  |%d  |%d  |%d  |0x%x|%c %c %c|\n",
            mnemonic, mnemonic, operands, type, jmp, ops, alu, ins, opcode, Z, C, O);
    }

}
