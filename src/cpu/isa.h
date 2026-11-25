/**
 * Follows the team design doc: RISC, 3 operands, destination leftmost,
 * R/I/J instruction types, Z/O/C flags, and the jmp|ops|alu|ins opcode fields.
 * Widened from the doc's 16-bit word to 32 bits; register fields are 4 bits (16 GPRs).
 *
 * Every instruction is one 32-bit word (bit 31 = leftmost):
 *
 *  31      24 23  20 19  16 15  12 11           0
 * +----------+------+------+------+--------------+
 * |  opcode  |  rd  |  rs1 |  rs2 |   unused     |   R-type
 * +----------+------+------+------+--------------+
 * |  opcode  |  rd  |  rs1 |     imm16           |   I-type
 * +----------+------+------+---------------------+
 * |  opcode  |         address24 (unsigned)      |   J-type
 * +----------+-----------------------------------+
 *
 * The opcode's format (ISA_TABLE[op].fmt) decides how bits 23..0 are read.
 * Example: ADD R1, R2, R3 = 0x71123000
 *
 * 2026-11-10 12:30 - Jehu Zepeda-Silva - isa.h - isa.h added (opcodes, formats, flags, API)
 * 2026-11-17 - Sriharsha Edukulla - isa.h - documented initial HALT/NOP support (#26)
 * 2026-11-22 - Chris Carl - isa.h - added OPCODES, isa_instruction_constructor, isa_decode is a pointer, isa_disassemble_2, and print_isa_table
 * 2026-11-23 - Chris Carl - isa.h - added FLAG_*_OF
 * 2026-11-23 - Chris Carl - isa.h - added INS_* numbers so each mnemonic is mapped to its instruction number
 *
 * Bare essentials: machine constants, field layout (shift/mask for
 * encode/decode), opcode fields and the opcode enum.
 *
 * After the opcode enum (not strictly needed, can mostly be computed on the fly):
 *
 *   isa_format - Which layout (NONE/R/I/J) to read bits 23..0 with.
 *   Same bits mean different things per format, so the decoder checks this first.
 *
 *   FLAG_Z, FLAG_C, FLAG_O - Bit masks for the Z/C/O flags in the FLAGS register.
 *
 *   MMIO_PUTCHAR, MMIO_PUTINT - Addresses where a STORE prints instead of writing to memory, so programs have a output.
 *
 *   isa_instruction - A decoded word as named fields (op, rd, rs1, rs2, imm, addr), so executor doesn't work with straight bits.
 *   (i.e just use REGISTER[inst.rd] = REGISTER[inst.rs1] + REGISTER[inst.rs2])
 *
 *   isa_opcode_info - One ISA_TABLE row: mnemonic (human readable instead of using 0x71, use OP_ADD to index), format (how bits are used), flags it sets,
 *   and whether imm16 is sign- or zero-extended.
 *
 *   ISA_TABLE - 256-entry (8 bits for opcode = 2^8 possible opcodes = 256) lookup indexed by opcode. Essentially Facts about each opcode live in one place;
 *   unused opcodes are all zeros (mnemonic == NULL). (ISA_TABLE[OP_ADD].flags_set  -> FLAG_Z|FLAG_O|FLAG_C)
 *
 *   Example, all together: 0x71123000
 *     op = word >> 24 = 0x71 -> ISA_TABLE[0x71] = {"ADD", FMT_R, Z|O|C}
 *     FMT_R -> decode gives {OP_ADD, rd=1, rs1=2, rs2=3} (isa_instruction)
 *     execute: R[1] = R[2] + R[3], then set FLAG_Z/FLAG_O/FLAG_C
 */

#ifndef H_ISA
#define H_ISA

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ---------- machine constants ---------- */
#define ISA_WORD_BITS 32
#define ISA_NUM_GPRS 16   // R0..R15 (4-bit register fields)
// 2 SPR ?
#define ISA_REG_LR 14   // link register (return address for CALL/RET)
#define ISA_REG_SP 15   // stack pointer (PUSH/POP); stack grows down

/* ---------- field layout ----------
 * extract: (word >> SHIFT) & MASK
 * insert:  (value & MASK) << SHIFT
 * Example: ADD R1, R2, R3 is the word 0x71123000.
 * 0x71123000 = 0111 0001 | 0001 | 0010 | 0011 | 0000 0000 0000
 *                opcode     rd     rs1    rs2    unused
 * (word >> 24) & 0xFF gives 0x71, the opcode for ADD.
 * (word >> 20) & 0xF gives 1, so rd is R1.
 * (word >> 16) & 0xF gives 2, so rs1 is R2.
 * (word >> 12) & 0xF gives 3, so rs2 is R3.
 */
#define ISA_OPCODE_SHIFT 24
#define ISA_RD_SHIFT 20
#define ISA_RS1_SHIFT 16
#define ISA_RS2_SHIFT 12   // imm16 and address24 have no shift, 0-11 are unused in R-type

#define ISA_OPCODE_MASK 0xFFu
#define ISA_REG_MASK 0xFu
#define ISA_IMM_MASK 0xFFFFu    // I-type immediate (bits 15..0)
#define ISA_ADDR_MASK 0xFFFFFFu  // J-type address   (bits 23..0)

/* ---------- opcode sub-fields (from the design doc) ----------
 *
 *   bit:   7     6  5     4     3  2  1  0
 *        +-----+------+-----+------------+
 *        | jmp | ops  | alu |    ins     |
 *        +-----+------+-----+------------+
 *
 * jmp: 1 = changes PC (jump, branch, call, return)
 * ops: number of operands (0..3)
 * alu: 1 = uses the ALU
 * ins: index within the group. ALU logic ops (AND..SRA) use ins >= 9,
 *      so "ops == 3 && alu && ins >= 9" means a pure bitwise op.
 *
 * The control unit can drive signals straight from these bits.
 */
#define ISA_OP(jmp, ops, alu, ins) \
    ((((jmp) & 1u) << 7) | (((ops) & 3u) << 5) | (((alu) & 1u) << 4) | ((ins) & 0xFu)) // Each argument shifted left into its slot and ORed into the 8-bit opcode.

#define ISA_OP_JMP(op) (((op) >> 7) & 1u)
#define ISA_OP_OPS(op) (((op) >> 5) & 3u)
#define ISA_OP_ALU(op) (((op) >> 4) & 1u)
#define ISA_OP_INS(op) ((op) & 0xFu)

#define OPCODES 256

/* ---------- opcodes ----------
 * Syntax is "MNEMONIC dest, src, src": destination leftmost.
 * give instructions names (rather then using hex values in the code)
 */
typedef enum {
    /* ----- control: 0 operands ----- */
    OP_HALT = ISA_OP(0, 0, 0, 0),   // 0x00 [added just in case] stop the clock
    OP_NOP = ISA_OP(0, 0, 0, 1),   // 0x01 [added just in case] do nothing
    OP_RET = ISA_OP(1, 0, 0, 0),   // 0x80 [added, not sure if we are using?] PC ← LR

    /* ----- stack: 1 operand ----- */
    OP_PUSH = ISA_OP(0, 1, 0, 0),   // 0x20 [added in case] R: SP ← SP - 1, mem[SP] ← rd
    OP_POP = ISA_OP(0, 1, 0, 1),   // 0x21 [added in case] R: rd ← mem[SP], SP ← SP + 1

    /* ----- jumps: 1 operand ----- */
    OP_JUMP = ISA_OP(1, 1, 0, 0),   // 0xA0 [from google doc]   J: PC ← address           (jump label)
    OP_JR  = ISA_OP(1, 1, 0, 1),   // 0xA1 [from google doc]   R: PC ← rd                (doc's "jump r1")
    OP_CALL = ISA_OP(1, 1, 0, 2),   // 0xA2 [from google doc]   J: LR ← PC, PC ← address  (stretch: JAL)

    /* ----- memory / move: 2 operands, no ALU ----- */
    OP_LOAD = ISA_OP(0, 2, 0, 0),   // 0x40 [doc]   I: rd ← mem[rs1 + imm]
    OP_STORE = ISA_OP(0, 2, 0, 1),   // 0x41 [doc]   I: mem[rd + imm] ← rs1   (rd holds the address)
    OP_MOV = ISA_OP(0, 2, 0, 2),   // 0x42 [doc]   R: rd ← rs1
    OP_LI = ISA_OP(0, 2, 0, 3),   // 0x43 [added] I: rd ← imm               (zero-extended)
    OP_LUI = ISA_OP(0, 2, 0, 4),   // 0x44 [added] I: rd ← imm << 16         (pair with OR for 32-bit constants)

    /* ----- unary ALU: 2 operands ----- */
    OP_NOT = ISA_OP(0, 2, 1, 0),   // 0x50 [doc]   R: rd ← ~rs1              (bitwise)
    OP_NEG = ISA_OP(0, 2, 1, 1),   // 0x51 [doc]   R: rd ← -rs1              (two's complement)

    /* ----- arithmetic: 3 operands ----- */
    OP_CMP = ISA_OP(0, 3, 1, 0),   // 0x70 [doc]   R: rd ← (rs1 - rs2 != 0) ? 1 : 0
    OP_ADD = ISA_OP(0, 3, 1, 1),   // 0x71 [doc]   R: rd ← rs1 + rs2
    OP_SUB = ISA_OP(0, 3, 1, 2),   // 0x72 [doc]   R: rd ← rs1 - rs2
    OP_MUL = ISA_OP(0, 3, 1, 3),   // 0x73 [doc]   R: rd ← low 32 bits of rs1 * rs2   (stretch)
    OP_DIV = ISA_OP(0, 3, 1, 4),   // 0x74 [doc]   R: rd ← rs1 / rs2   (stretch) TODO: divide by zero
    OP_ADDI = ISA_OP(0, 3, 1, 5),   // 0x75 [added] I: rd ← rs1 + imm         (imm sign-extended)

    /* ----- logic: 3 operands, ins >= 9 ----- */
    OP_AND = ISA_OP(0, 3, 1, 9),   // 0x79 [doc]   R: rd ← rs1 & rs2
    OP_OR = ISA_OP(0, 3, 1, 10),  // 0x7A [doc]   R: rd ← rs1 | rs2
    OP_XOR = ISA_OP(0, 3, 1, 11),  // 0x7B [doc]   R: rd ← rs1 ^ rs2
    OP_SL = ISA_OP(0, 3, 1, 12),  // 0x7C [doc]   R: rd ← rs1 << rs2        (zero-fill)
    OP_SR = ISA_OP(0, 3, 1, 13),  // 0x7D [doc]   R: rd ← rs1 >> rs2        (logical, zero-fill)
    OP_SLA = ISA_OP(0, 3, 1, 14),  // 0x7E [doc]   R: rd ← rs1 << rs2        (same result as SL)
    OP_SRA = ISA_OP(0, 3, 1, 15),  // 0x7F [doc]   R: rd ← rs1 >> rs2        (arithmetic, copies sign bit)

    /* ----- branches: compare rd with rs1, then PC ← imm (absolute, 0..65535) ----- */
    OP_BEQ = ISA_OP(1, 3, 1, 0),   // 0xF0 [doc]   I: if rd == rs1
    OP_BNQ = ISA_OP(1, 3, 1, 1),   // 0xF1 [doc]   I: if rd != rs1
    OP_BLT = ISA_OP(1, 3, 1, 2),   // 0xF2 [doc]   I: if rd <  rs1  (signed)
    OP_BGT = ISA_OP(1, 3, 1, 3),   // 0xF3 [doc]   I: if rd >  rs1  (signed)
    OP_BLE = ISA_OP(1, 3, 1, 4),   // 0xF4 [doc]   I: if rd <= rs1  (signed)
    OP_BGE = ISA_OP(1, 3, 1, 5),   // 0xF5 [doc]   I: if rd >= rs1  (signed)
} isa_opcode;

typedef enum {
    /* ----- control: 0 operands ----- */
    INS_HALT = 0,
    INS_NOP = 1,
    INS_RET = 0,

    /* ----- stack: 1 operand ----- */
    INS_PUSH = 0,
    INS_POP = 1,

    /* ----- jumps: 1 operand ----- */
    INS_JUMP = 0,
    INS_JR  = 1,
    INS_CALL = 2,

    /* ----- memory / move: 2 operands, no ALU ----- */
    INS_LOAD = 0,
    INS_STORE = 1,
    INS_MOV = 2,
    INS_LI = 3,
    INS_LUI = 4,

    /* ----- unary ALU: 2 operands ----- */
    INS_NOT = 0,
    INS_NEG = 1,

    /* ----- arithmetic: 3 operands ----- */
    INS_CMP = 0,
    INS_ADD = 1,
    INS_SUB = 2,
    INS_MUL = 3,
    INS_DIV = 4,
    INS_ADDI = 5,

    /* ----- logic: 3 operands, ins >= 9 ----- */
    INS_AND = 9,
    INS_OR = 10,
    INS_XOR = 11,
    INS_SL = 12,
    INS_SR = 13,
    INS_SLA = 14,
    INS_SRA = 15,

    /* ----- branches: compare rd with rs1, then PC ← imm (absolute, 0..65535) ----- */
    INS_BEQ = 0,
    INS_BNQ = 1,
    INS_BLT = 2,
    INS_BGT = 3,
    INS_BLE = 4,
    INS_BGE = 5,
} isa_ins;

/* ---------- formats: how to read bits 23..0 ---------- */
typedef enum {
    FMT_NONE,  // no operands (HALT, NOP, RET)
    FMT_R,     // rd, rs1, rs2          (unused register fields are 0)
    FMT_I,     // rd, rs1, imm16
    FMT_J,     // address24
} isa_format;

/* ---------- status flags (bits in the FLAGS register) ----------
 * Replaces flags_t in ip/registers.h; use one or the other.
 */
#define FLAG_Z (1u << 0)  // zero: result == 0
#define FLAG_Z_OF(value) ((value & FLAG_Z) >> 0)
#define FLAG_C (1u << 1)  // carry: unsigned overflow
#define FLAG_C_OF(value) ((value & FLAG_C) >> 1)
#define FLAG_O (1u << 2)  // overflow: signed overflow
#define FLAG_O_OF(value) ((value & FLAG_O) >> 2)
#define FLAG_NONE (0u)    // no flags are set high at all

/* ---------- memory-mapped I/O (STORE to these prints) ---------- */
#define MMIO_PUTCHAR 0xFF00  // print low byte as ASCII
#define MMIO_PUTINT 0xFF01  // print as signed decimal

/* ---------- decoded instruction (output of isa_decode) ----------
 * Unused fields are 0.
 */
typedef struct {
    isa_opcode op;
    uint8_t rd;    // destination; a source for PUSH, JR and branches; address for STORE
    uint8_t rs1;
    uint8_t rs2;   // R-type only
    int32_t imm;   // I-type only, extended per ISA_TABLE[op].imm_signed
    uint32_t addr;  // J-type only
} isa_instruction;

/* ---------- per-opcode metadata (one row per opcode) ---------- */
typedef struct {
    const char* mnemonic;    // e.g. "ADD"; NULL = invalid opcode
    isa_format fmt;
    uint8_t flags_set;   // FLAG_* bits this op updates
    bool imm_signed;  // I-type: sign-extend imm16 (false = zero-extend)
    bool valid;  // false--so MOST of ISA_TABLE will have !valid ISA opcode info
} isa_opcode_info;

/* Indexed by opcode
 *
 * Flags each op sets (per the design doc, plus C on add/sub/neg):
 *   ADD, SUB, ADDI, NEG ........... Z O C
 *   everything else in the ALU .... Z
 *   branches ...................... Z   (from their internal compare)
 *   memory, move, jumps, stack .... none
 */
extern const isa_opcode_info ISA_TABLE[OPCODES];

/* ---------- API ---------- */
bool isa_is_valid(uint8_t opcode);
isa_instruction* isa_instruction_constructor();
isa_instruction* isa_decode(uint32_t word);                            // word → fields
uint32_t isa_encode(isa_instruction inst);                     // fields → word
const char* isa_mnemonic(uint8_t opcode);                         // "ADD" or NULL
const char* isa_operands(uint8_t opcode, uint8_t instruction_format);   // switch on the format, FMT_I, FMT_J, etc.
void isa_disassemble(uint32_t word, char* buf, size_t buflen);
void isa_disassemble_2(uint32_t word, char* buf, size_t buflen);  // alternate way of doing isa_disassemble
void print_isa_table();

#endif
