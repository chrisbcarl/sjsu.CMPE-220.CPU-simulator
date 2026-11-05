/**
 * isa.h: instruction set definition (opcodes, formats, flags, memory map).
 * Single source of truth. The decoder, executor, assembler and docs must match it.
 * Holds no CPU state; that lives in cpu/.
 *
 * Every instruction is one 32-bit word:
 *
 *  31      24 23  20 19  16 15  12 11           0
 * +----------+------+------+------+--------------+
 * |  opcode  |  rd  |  rs1 |  rs2 |   unused     |   R-type
 * +----------+------+------+------+--------------+
 * |  opcode  |  rd  |  rs1 |     imm16 (signed)  |   I-type
 * +----------+------+------+---------------------+
 *
 * imm16 shares bits 15..0 with rs2; the opcode's format decides which.
 * Example: ADD R1, R2, R3 = 0x05123000
 */
#ifndef H_ISA
#define H_ISA

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ---------- machine constants ---------- */
#define ISA_NUM_GPRS   16   // R0..R15
#define ISA_REG_LR     14   // link register (return address for CALL/RET)
#define ISA_REG_SP     15   // stack pointer (PUSH/POP); stack grows down

/* ---------- field layout ----------
 * extract: (word >> SHIFT) & MASK
 * insert:  (value & MASK) << SHIFT
 */
#define ISA_OPCODE_SHIFT  24
#define ISA_RD_SHIFT      20
#define ISA_RS1_SHIFT     16
#define ISA_RS2_SHIFT     12   // imm16 has no shift (bits 15..0)

#define ISA_OPCODE_MASK   0xFFu
#define ISA_REG_MASK      0xFu
#define ISA_IMM_MASK      0xFFFFu  // sign-extend after masking: 0xFFFF = -1

/* ---------- opcodes ----------*/
typedef enum {
    /* control */
    OP_HALT = 0x00,  // stop the clock
    OP_NOP  = 0x01,  // do nothing

    /* data movement */
    OP_MOV  = 0x02,  // R: rd ← rs1
    OP_LD   = 0x03,  // I: rd ← mem[rs1 + imm]
    OP_ST   = 0x04,  // I: mem[rs1 + imm] ← rd   (rd is the source)
    OP_LI   = 0x11,  // I: rd ← imm               (sign-extended)
    OP_LUI  = 0x12,  // I: rd ← imm << 16         (pair with ADDI/OR for 32-bit constants)

    /* arithmetic (flags: ZNCV for ADD/SUB/ADDI, ZN for MUL/DIV) */
    OP_ADD  = 0x05,  // R: rd ← rs1 + rs2
    OP_SUB  = 0x06,  // R: rd ← rs1 - rs2
    OP_MUL  = 0x07,  // R: rd ← low 32 bits of rs1 * rs2
    OP_DIV  = 0x08,  // R: rd ← rs1 / rs2         TODO: divide-by-zero behavior
    OP_ADDI = 0x13,  // I: rd ← rs1 + imm

    /* logic (flags: ZN) */
    OP_AND  = 0x09,  // R: rd ← rs1 & rs2
    OP_OR   = 0x0A,  // R: rd ← rs1 | rs2
    OP_XOR  = 0x0B,  // R: rd ← rs1 ^ rs2
    OP_NOT  = 0x0C,  // R: rd ← ~rs1

    /* shifts (flags: ZNC) */
    OP_SHL  = 0x0D,  // R: rd ← rs1 << rs2
    OP_SHR  = 0x0E,  // R: rd ← rs1 >> rs2        (logical, zero-fill)

    /* compare (flags: ZNCV) */
    OP_CMP  = 0x0F,  // R: flags ← rs1 - rs2      (result discarded)

    /* jumps: PC ← imm (absolute address) */
    OP_JMP  = 0x10,  // I: always
    OP_JZ   = 0x14,  // I: if Z   (equal, after CMP)
    OP_JNZ  = 0x15,  // I: if !Z  (not equal)
    OP_JN   = 0x16,  // I: if N   (less than, signed)

    /* subroutines and stack */
    OP_CALL = 0x17,  // I: LR ← PC, PC ← imm
    OP_RET  = 0x18,  //    PC ← LR
    OP_PUSH = 0x19,  // R: SP ← SP - 1, mem[SP] ← rd
    OP_POP  = 0x1A,  // R: rd ← mem[SP], SP ← SP + 1
} isa_opcode;

/* ---------- formats: how to read bits 15..0 ---------- */
typedef enum {
    FMT_NONE,  // no operands (HALT, NOP, RET)
    FMT_R,     // rd, rs1, rs2
    FMT_I,     // rd, rs1, imm16
} isa_format;

/* ---------- status flags (bits in the FLAGS register) ----------
 * Replaces flags_t in ip/registers.h; use one or the other.
 */
#define FLAG_Z  (1u << 0)  // zero: result == 0
#define FLAG_N  (1u << 1)  // negative: bit 31 set
#define FLAG_C  (1u << 2)  // carry: unsigned overflow
#define FLAG_V  (1u << 3)  // overflow: signed overflow

/* ---------- memory-mapped I/O (ST to these prints) ---------- */
#define MMIO_PUTCHAR  0xFF00  // print low byte as ASCII
#define MMIO_PUTINT   0xFF01  // print as signed decimal

/* ---------- decoded instruction (output of isa_decode) ----------
 * Unused fields are 0.
 */
typedef struct {
    isa_opcode op;
    uint8_t    rd;   // destination (source for ST/PUSH)
    uint8_t    rs1;
    uint8_t    rs2;  // R-type only
    int32_t    imm;  // I-type only, sign-extended
} isa_instruction;

/* ---------- per-opcode metadata (one row per opcode) ---------- */
typedef struct {
    const char* mnemonic;   // e.g. "ADD"; NULL = invalid opcode
    isa_format  fmt;
    uint8_t     flags_set;  // FLAG_* bits this op updates
} isa_opcode_info;

/* Indexed by opcode. Defined in isa.c; unlisted entries are zero (invalid). */
extern const isa_opcode_info ISA_TABLE[256];

/* ---------- API ---------- */
bool isa_is_valid(uint8_t opcode);                          // opcode has a table row?
isa_instruction isa_decode(uint32_t word);                             // word → fields
uint32_t isa_encode(isa_instruction inst);                      // fields → word
const char* isa_mnemonic(uint8_t opcode);                          // "ADD" or NULL
void isa_disassemble(uint32_t word, char* buf, size_t len); // "ADD R1, R2, R3"

#endif /* H_ISA */