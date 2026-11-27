/*
 * 2026-11-17 - Sriharsha Edukulla - test_isa.c - checks for initial HALT/NOP validation and disassembly (#26)
 * 2026-11-20 - Sriharsha Edukulla - test_isa.c - ADD, POP and LOAD examples from PR review
 * 2026-11-22 - Chris Carl - test_isa.c - isa overhaul in prep for the instructions that we're implementing, need to check that they're done correctly
 * 2026-11-23 - Chris Carl - test_isa.c - added LI, LUI, ADD
 * 2026-11-25 - Sriharsha Edukulla - restored opcode and buffer checks (#67)
 *
 * Build and run from the repository root:
 * mkdir -p bin
 * cc -std=c11 -Wall -Wextra -I./src/utilities src/utilities/common.c -Isrc/cpu src/cpu/isa.c test/isa/test_isa.c -o bin/test_isa
 * ./bin/test_isa
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"  // COMMON_BUFFER_0, assert
#include "isa.h"

typedef struct {
    uint32_t word;
    const char* text;
    bool legacy_supported;
} disassembly_case;

// Literal words let us check the bit layout without relying on isa_encode.
static const disassembly_case valid_instructions[] = {
    {0x00000000, "HALT", true},
    {0x01000000, "NOP", true},
    {0x71123000, "ADD R1, R2, R3", true},
    {0x71DEF000, "ADD R13, R14, R15", true},
    {0x21C00000, "POP R12", true},
    {0x40FD0045, "LOAD R15, 69(R13)", true},
    {0x43A06969, "LI R10, 26985", false},
    {0x44306969, "LUI R3, 26985", false},
};
size_t valid_instruction_count = sizeof(valid_instructions) / sizeof(valid_instructions[0]);
static const uint32_t invalid_instructions[] = {
    0xDEADBEEF,
    0xDEAD0000,
    0xFFFFFFFF,
    0xFF000000,
    0x69000000,
    0x69000FF,
};
size_t invalid_instruction_count = sizeof(invalid_instructions) / sizeof(invalid_instructions[0]);

static unsigned int failures = 0;

static void check_disassembly(uint32_t word, const char* expected) {
    char buf[64];
    isa_disassemble(word, buf, sizeof(buf));
    if (strcmp(buf, expected) != 0) {
        fprintf(stderr, "0x%08" PRIX32 ": expected '%s', got '%s'\n", word, expected, buf);
        failures++;
    }

    // Try every buffer size, including zero, one byte and an exact fit.
    // The '!' bytes on both sides should survive every call.
    size_t text_len = strlen(expected);
    for (size_t size = 0; size <= text_len + 2; size++) {
        char guarded[80];
        memset(guarded, '!', sizeof(guarded));
        char* out = guarded + 1;
        isa_disassemble(word, out, size);
        size_t prefix = size == 0 ? 0 : size - 1;
        if (prefix > text_len) {
            prefix = text_len;
        }

        bool ok = guarded[0] == '!';
        for (size_t i = size + 1; i < sizeof(guarded); i++) {
            ok = ok && guarded[i] == '!';
        }
        if (size > 0) {
            ok = ok && strncmp(out, expected, prefix) == 0 && out[prefix] == '\0';
        }
        if (!ok) {
            fprintf(stderr, "0x%08" PRIX32 ": bad output or overwrite with buffer size %zu\n", word, size);
            failures++;
        }
    }
}

static void test_sriharsha(void) {
    fprintf(stdout, "    INFO: test_sriharsha() - legacy disassembly and buffer sizes\n");
    bool legacy_supported[OPCODES] = {false};
    for (size_t i = 0; i < valid_instruction_count; i++) {
        const disassembly_case* example = &valid_instructions[i];
        if (example->legacy_supported) {
            legacy_supported[example->word >> ISA_OPCODE_SHIFT] = true;
            check_disassembly(example->word, example->text);
        }
    }

    // The legacy formatter falls back even for valid opcodes it doesn't format.
    for (unsigned int opcode = 0; opcode < OPCODES; opcode++) {
        if (!legacy_supported[opcode]) {
            uint32_t word = ((uint32_t)opcode << ISA_OPCODE_SHIFT) | 0x123456;
            char fallback[32];
            snprintf(fallback, sizeof(fallback), "UNSUPPORTED 0x%08" PRIX32, word);
            check_disassembly(word, fallback);
        }
    }

    // Unused bits should not become extra operands.
    check_disassembly(0x00FFFFFF, "HALT");
    check_disassembly(0x01FFFFFF, "NOP");
    check_disassembly(0x71F0FFFF, "ADD R15, R0, R15");
    check_disassembly(0x710F0000, "ADD R0, R15, R0");
    check_disassembly(0x21CFFFFF, "POP R12");
    check_disassembly(0x21000000, "POP R0");
    check_disassembly(0x21FFFFFF, "POP R15");

    // LOAD keeps the full offset instead of treating bits 15..12 as rs2.
    check_disassembly(0x40000000, "LOAD R0, 0(R0)");
    check_disassembly(0x40A21234, "LOAD R10, 4660(R2)");
    check_disassembly(0x40FFFFFF, "LOAD R15, 65535(R15)");
    check_disassembly(0x41A30000, "UNSUPPORTED 0x41A30000");
    check_disassembly(0xFFFFFFFF, "UNSUPPORTED 0xFFFFFFFF");
    isa_disassemble(0x00000000, NULL, 0);
    isa_disassemble(0x01000000, NULL, 16);
    isa_disassemble(0x40FD0045, NULL, 16);
}

int test_isa_is_valid(void) {
    // Use the opcode bytes from docs/isa.md so this checks the full ISA.
    static const uint8_t defined_opcodes[] = {
        0x00, 0x01, 0x80,                         // HALT, NOP, RET
        0x20, 0x21,                               // stack
        0xA0, 0xA1, 0xA2,                         // jumps
        0x40, 0x41, 0x42, 0x43, 0x44,             // memory, move, immediates
        0x50, 0x51,                               // unary ALU
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75,       // arithmetic
        0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, // logic and shifts
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5,       // branches
    };
    bool expected[OPCODES] = {false};
    for (size_t i = 0; i < sizeof(defined_opcodes) / sizeof(defined_opcodes[0]); i++) {
        expected[defined_opcodes[i]] = true;
    }

    fprintf(stdout, "    INFO: test_isa_is_valid() - all %u opcode values\n", (unsigned int)OPCODES);
    for (unsigned int opcode = 0; opcode < OPCODES; opcode++) {
        if (isa_is_valid((uint8_t)opcode) != expected[opcode]) {
            fprintf(stderr, "Wrong validity for opcode 0x%02X (expected %s)\n",
                    opcode, expected[opcode] ? "valid" : "invalid");
            failures++;
        }
    }
    return EXIT_SUCCESS;
}

int test_isa_decode(void) {
    fprintf(stdout, "    INFO: test_isa_decode()\n");
    for (size_t i = 0; i < valid_instruction_count; i++) {
        uint32_t word = valid_instructions[i].word;
        const char* text = valid_instructions[i].text;
        isa_instruction* instr = isa_decode(word);
        assert(instr != NULL, "ERROR: valid_instructions[%lu] = '%s' could not create an isa instruction struct!\n", i, text);
        free(instr);
    }
    for (size_t i = 0; i < invalid_instruction_count; i++) {
        uint32_t word = invalid_instructions[i];
        isa_instruction* instr = isa_decode(word);
        assert(instr == NULL, "ERROR: isa_decode - invalid_instructions[%lu] = 0x%x is supposed to be invalid!\n", i, word);
        free(instr);
    }

    return EXIT_SUCCESS;
}

int test_isa_encode(void) {
    fprintf(stdout, "    INFO: test_isa_encode()\n");
    for (size_t i = 0; i < valid_instruction_count; i++) {
        uint32_t word = valid_instructions[i].word;
        const char* text = valid_instructions[i].text;
        isa_instruction* instr = isa_decode(word);
        printf("        valid_instrs[%lu], '%s' - op: 0x%x | rd: 0x%x | rs1: 0x%x | rs2: 0x%x | imm: 0x%x | addr: 0x%x\n", i, text, instr->op, instr->rd, instr->rs1, instr->rs2, instr->imm, instr->addr);

        uint32_t word2 = isa_encode(instr);
        assert(word == word2, "ERROR: isa_encode - decoded word[%lu] 0x%x != encoded 0x%x!\n", i, word, word2);
        free(instr);
    }

    return EXIT_SUCCESS;
}

int test_isa_mnemonic(void) {
    fprintf(stdout, "    INFO: test_isa_mnemonic()\n");
    for (size_t i = 0; i < valid_instruction_count; i++) {
        uint32_t word = valid_instructions[i].word;
        const char* text = valid_instructions[i].text;

        const char* mnemonic = isa_mnemonic(word >> 24);

        // copy text's mnemonic to buffer, compare to mnemonic
        size_t n = strlen(text);
        COMMON_BUFFER_0[n] = '\0';  // NOTE: give COMMON_BUFFER_0 a null termination otherwise compiler freaks out
        strncpy(COMMON_BUFFER_0, text, n);
        int idx_space = indexof((char*)text, ' ');

        // cannot use ternary statement due to int/size_t conversions
        if (idx_space == -1) n = strlen(mnemonic);
        else n = idx_space - 1;
        assert(strncmp(mnemonic, COMMON_BUFFER_0, n) == 0, "ERROR: isa_mnemonic - valid[%lu] '%s' != '%s'!\n", i, COMMON_BUFFER_0, mnemonic);
    }

    return EXIT_SUCCESS;
}

int test_isa_operands(void) {
    fprintf(stdout, "    INFO: test_isa_operands()\n");
    for (size_t i = 0; i < valid_instruction_count; i++) {
        uint32_t word = valid_instructions[i].word;
        const char* text = valid_instructions[i].text;

        isa_opcode_info info = ISA_TABLE[word >> 24];
        const char* operands = isa_operands(word >> 24, info.fmt);

        // copy text's operands to buffer, compare to operands
        size_t n = strcnt((char*)text, ' ');
        size_t m = strlen(operands) == 0 ? 0 : strcnt((char*)operands, ' ') + 1;
        // compare if number of spaces is the same (trite, i know)
        assert(n == m, "ERROR: isa_operands - valid[%lu] operand count not the same! Spaces count '%s' != '%s'! %lu != %lu!\n", i, text, operands, n, m);
    }

    return EXIT_SUCCESS;
}

int test_isa_disassemble_2(void) {
    fprintf(stdout, "    INFO: test_isa_disassemble_2()\n");
    for (size_t i = 0; i < valid_instruction_count; i++) {
        uint32_t word = valid_instructions[i].word;
        const char* text = valid_instructions[i].text;

        isa_disassemble_2(word, COMMON_BUFFER_0, COMMON_BUFFER_LEN);
        printf("        0x%x -> '%s'\n", word, COMMON_BUFFER_0);
        assert(strcmp(COMMON_BUFFER_0, text) == 0, "ERROR: isa_disassemble_2 - valid[%lu] '%s' != '%s'!\n", i, text, COMMON_BUFFER_0);
    }

    return EXIT_SUCCESS;
}

int main(void) {
    fprintf(stdout, "test_isa\n");
    print_isa_table();
    test_isa_is_valid();
    test_sriharsha();
    test_isa_decode();
    test_isa_encode();
    test_isa_mnemonic();
    test_isa_operands();
    test_isa_disassemble_2();

    if (failures != 0) {
        fprintf(stderr, "FAILED! %u failures!\n", failures);
        return EXIT_FAILURE;
    }
    printf("PASSED on %lu valid instructions, %lu invalid instructions!\n", valid_instruction_count, invalid_instruction_count);
    return EXIT_SUCCESS;
}
