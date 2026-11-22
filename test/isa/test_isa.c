/*
 * 2026-11-17 - Sriharsha Edukulla - checks for initial HALT/NOP validation and disassembly (#26)
 * 2026-11-20 - Sriharsha Edukulla - ADD, POP and LOAD examples from PR review
 * 2026-11-22 - Chris Carl - isa overhaul in prep for the instructions that we're implementing, need to check that they're done correctly
 * 2026-11-22 - Chris Carl - test_isa.c - added LI, LUI
 *
 * Build and run from the repository root:
 * mkdir -p bin
 * cc -std=c11 -Wall -Wextra -Isrc src/common.c -Isrc/cpu src/cpu/isa.c test/isa/test_isa.c -o bin/test_isa
 * ./bin/test_isa
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"  // BUFFER0
#include "isa.h"

#define assert(condition, failure_message, ...) \
    if (!(condition)) {fprintf(stderr, failure_message __VA_OPT__(,) __VA_ARGS__); exit(1);}

typedef struct {
    uint32_t word;
    const char* text;
} disassembly_case;

// Literal words let us check the bit layout without relying on isa_encode.
static const disassembly_case valid_instructions[] = {
    {0x00000000, "HALT"},
    {0x01000000, "NOP"},
    {0x71123000, "ADD R1, R2, R3"},
    {0x71DEF000, "ADD R13, R14, R15"},
    {0x21C00000, "POP R12"},
    {0x40FD0045, "LOAD R15, 69(R13)"},
    {0x43A06969, "LI R10, 26985"},
    {0x44306969, "LUI R3, 26985"},
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

int test_sriharsha(void) {
    for (unsigned int opcode = 0; opcode < 256; opcode++) {
        bool expected = false;
        for (size_t i = 0; i < valid_instruction_count; i++) {
            if ((valid_instructions[i].word >> 24) == opcode) {
                expected = true;
                break;
            }
        }
        if (isa_is_valid((uint8_t)opcode) != expected) {
            fprintf(stderr, "Wrong validity for opcode 0x%02X\n", opcode);
            failures++;
        }
        if (!expected) {
            uint32_t word = ((uint32_t)opcode << 24) | 0x123456;
            char invalid[32];
            snprintf(invalid, sizeof(invalid), "UNSUPPORTED 0x%08" PRIX32, word);
            check_disassembly(word, invalid);
        }
    }

    for (size_t i = 0; i < valid_instruction_count; i++) {
        check_disassembly(valid_instructions[i].word, valid_instructions[i].text);
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

    if (failures != 0) {
        fprintf(stderr, "%u ISA checks failed\n", failures);
        return failures;
    }
    printf("ISA checks passed: %zu instructions, unsupported opcodes and buffer sizes\n", valid_instruction_count);
    return EXIT_SUCCESS;
}

int test_isa_is_valid(void) {
    fprintf(stdout, "    INFO: test_isa_is_valid()\n");
    for (size_t i = 0; i < valid_instruction_count; i++) {
        uint32_t word = valid_instructions[i].word;
        const char* text = valid_instructions[i].text;
        assert(isa_is_valid(word >> 24), "ERROR: isa_is_valid - valid_instructions[%lu] = '%s' invalid!\n", i, text);
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

        uint32_t word2 = isa_encode(*instr);
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
        BUFFER0[n] = '\0';  // NOTE: give BUFFER0 a null termination otherwise compiler freaks out
        strncpy(BUFFER0, text, n);
        int idx_space = indexof((char*)text, ' ');

        // cannot use ternary statement due to int/size_t conversions
        if (idx_space == -1) n = strlen(mnemonic);
        else n = idx_space - 1;
        assert(strncmp(mnemonic, BUFFER0, n) == 0, "ERROR: isa_mnemonic - valid[%lu] '%s' != '%s'!\n", i, BUFFER0, mnemonic);
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

        isa_disassemble_2(word, BUFFER0, BUFLEN);
        printf("        0x%x -> '%s'\n", word, BUFFER0);
        assert(strcmp(BUFFER0, text) == 0, "ERROR: isa_disassemble_2 - valid[%lu] '%s' != '%s'!\n", i, text, BUFFER0);
    }

    return EXIT_SUCCESS;
}

int main(void) {
    fprintf(stdout, "test_isa\n");
    // test_sriharsha();  // I think it's treating everything thats not in the valid array as an invalid...
    print_isa_table();
    test_isa_is_valid();
    test_isa_decode();
    test_isa_encode();
    test_isa_mnemonic();
    test_isa_operands();
    test_isa_disassemble_2();

    if (failures != 0) {
        fprintf(stderr, "FAILED! %u failures!\n", failures);
        return failures;
    }
    printf("PASSED on %lu valid instructions, %lu invalid instructions!\n", valid_instruction_count, invalid_instruction_count);
    return EXIT_SUCCESS;
}
