/*
 * 2026-11-17 - Sriharsha Edukulla - checks for initial HALT/NOP validation and disassembly (#26)
 * 2026-11-20 - Sriharsha Edukulla - ADD, POP and LOAD examples from PR review
 *
 * Build and run from the repository root:
 * mkdir -p bin
 * cc -std=c11 -Wall -Wextra -I src/cpu src/cpu/isa.c test/isa/test_isa.c -o bin/test_isa
 * ./bin/test_isa
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isa.h"

typedef struct {
    uint32_t word;
    const char* text;
} disassembly_case;

// Literal words let us check the bit layout without relying on isa_encode.
static const disassembly_case instructions[] = {
    {0x00000000, "HALT"},
    {0x01000000, "NOP"},
    {0x71123000, "ADD R1, R2, R3"},
    {0x21C00000, "POP R12"},
    {0x40FD0045, "LOAD R15, 69(R13)"},
};

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

int main(void) {
    size_t instruction_count = sizeof(instructions) / sizeof(instructions[0]);
    for (unsigned int opcode = 0; opcode < 256; opcode++) {
        bool expected = false;
        for (size_t i = 0; i < instruction_count; i++) {
            if ((instructions[i].word >> 24) == opcode) {
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

    for (size_t i = 0; i < instruction_count; i++) {
        check_disassembly(instructions[i].word, instructions[i].text);
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
        return EXIT_FAILURE;
    }
    printf("ISA checks passed: %zu instructions, unsupported opcodes and buffer sizes\n", instruction_count);
    return EXIT_SUCCESS;
}
