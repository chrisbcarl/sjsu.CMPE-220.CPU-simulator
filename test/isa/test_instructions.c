/*
 * 2026-11-23 - Chris Carl - test_instructions.c - added ADD, LI, LUI, flag test cases
 *
 * mkdir -p bin
 * cc -std=c11 -Wall -Wextra -I./src/utilities src/utilities/common.c -I./src/cpu src/cpu/isa.c src/cpu/instructions.c test/isa/test_instructions.c -o bin/test_instructions
 * ./bin/test_instructions
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"  // BUFFER0, assert
#include "isa.h"
#include "instructions.h"

void print_flags(uint8_t flags) {
    printf("    flags: 0x%x | Z: %01d | C: %01d | O: %01d\n", flags, FLAG_Z_OF(flags), FLAG_C_OF(flags), FLAG_O_OF(flags));
}

int test_memory(void) {
    fprintf(stdout, "    INFO: test_memory()\n");
    uint32_t r1; // , r2, rd;
    uint16_t imm;
    // uint32_t addr;
    // uint8_t flags;

    r1 = 69;
    imm = 420;
    LI(&r1, &imm);
    assert(r1 == 420, "how is that even possible not to be the case...?\n");

    LUI(&r1, &imm);
    assert(r1 == (420 * 0x10000 + 420), "how is that even possible to go so bad...?\n");

    return EXIT_SUCCESS;
}

int test_arithmetic(void) {
    fprintf(stdout, "    INFO: test_arithmetic()\n");
    uint32_t rd, r1, r2;
    // uint16_t imm;
    // uint32_t addr;
    uint8_t flags = 0;

    r1 = 0x1, r2 = 0x2;
    ADD(&rd, &r1, &r2, &flags);
    assert(rd == 3, "how is that even possible to screw up?\n");
    assert(flags == 0, "i just added 2 numbers, how can the flags go up?\n");
    ADD(&rd, &r2, &r1, &flags);
    assert(rd == 3, "how is that even possible to screw up? swap\n");
    assert(flags == 0, "i just added 2 numbers, how can the flags go up? swap\n");

    r1 = 0xFFFFFFFF, r2 = 0x1;
    ADD(&rd, &r1, &r2, &flags);
    assert(rd == 0, "0xFFFFFFFF + 1 == 0 w/ C right?\n");
    assert(FLAG_Z_OF(flags) && FLAG_C_OF(flags) && !FLAG_O_OF(flags), "0xFFFFFFFF + 1 -> Z high, C high, O low, right?\n");
    ADD(&rd, &r2, &r1, &flags);
    assert(rd == 0, "0xFFFFFFFF + 1 == 0 w/ C right? swap\n");
    assert(FLAG_Z_OF(flags) && FLAG_C_OF(flags) && !FLAG_O_OF(flags), "0xFFFFFFFF + 1 -> Z high, C high, O low, right? swap\n");

    r1 = 0x7FFFFFFF, r2 = 0x1;  // 2 positive, go negative
    ADD(&rd, &r1, &r2, &flags);
    assert(rd == 0x80000000, "0x7FFFFFFF + 1 == 0x80000000 right?\n");
    assert(!FLAG_Z_OF(flags) && !FLAG_C_OF(flags) && FLAG_O_OF(flags), "0x7FFFFFFF + 1 -> Z low, C low, O high, right?\n");
    ADD(&rd, &r2, &r1, &flags);
    assert(rd == 0x80000000, "0x7FFFFFFF + 1 == 0x80000000 right? swap\n");
    assert(!FLAG_Z_OF(flags) && !FLAG_C_OF(flags) && FLAG_O_OF(flags), "0x7FFFFFFF + 1 -> Z low, C low, O high, right? swap\n");

    r1 = 0x7FFFFFFF, r2 = 0x7FFFFFFF;  // 2 positive, go negative
    ADD(&rd, &r1, &r2, &flags);
    assert(rd == 0xFFFFFFFE, "0x7FFFFFFF + 0x7FFFFFFF == 0xFFFFFFFE right?\n");
    assert(!FLAG_Z_OF(flags) && !FLAG_C_OF(flags) && FLAG_O_OF(flags), "0x7FFFFFFF + 0x7FFFFFFF -> Z low, C low, O high right?\n");
    ADD(&rd, &r2, &r1, &flags);
    assert(rd == 0xFFFFFFFE, "0x7FFFFFFF + 0x7FFFFFFF == 0xFFFFFFFE right? swap\n");
    assert(!FLAG_Z_OF(flags) && !FLAG_C_OF(flags) && FLAG_O_OF(flags), "0x7FFFFFFF + 0x7FFFFFFF -> Z low, C low, O high right? swap\n");

    r1 = 0xFFFFFFFF, r2 = 0x2;  // 1 negative, 1 positive, go positive
    ADD(&rd, &r1, &r2, &flags);
    assert(rd == 0x00000001, "0xFFFFFFFF + 0x2 == 0x00000001 right?\n");
    assert(!FLAG_Z_OF(flags) && FLAG_C_OF(flags) && !FLAG_O_OF(flags), "0xFFFFFFFF + 0x2 -> Z low, C high, O low, right?\n");
    ADD(&rd, &r2, &r1, &flags);  // swap order
    assert(rd == 0x00000001, "0xFFFFFFFF + 0x2 == 0x00000001 right? swap\n");
    assert(!FLAG_Z_OF(flags) && FLAG_C_OF(flags) && !FLAG_O_OF(flags), "0xFFFFFFFF + 0x2 -> Z low, C high, O low, right? swap\n");

    r1 = 0xFFFFFFFE, r2 = 0x1;  // 1 negative, 1 positive, go negative
    ADD(&rd, &r1, &r2, &flags);
    assert(rd == 0xFFFFFFFF, "0xFFFFFFFE + 0x1 == 0x00000000 right?\n");
    assert(!FLAG_Z_OF(flags) && !FLAG_C_OF(flags) && !FLAG_O_OF(flags), "0xFFFFFFFE + 0x1 -> Z low, C low, O low, right?\n");
    ADD(&rd, &r2, &r1, &flags);  // swap order
    assert(rd == 0xFFFFFFFF, "0xFFFFFFFE + 0x1 == 0x00000000 right? swap\n");
    assert(!FLAG_Z_OF(flags) && !FLAG_C_OF(flags) && !FLAG_O_OF(flags), "0xFFFFFFFE + 0x1 -> Z low, C low, O low, right? swap\n");

    // printf("rd: 0x%x, flags: 0x%x\n", rd, flags);
    // print_flags(flags);

    return EXIT_SUCCESS;
}

int main(void) {
    fprintf(stdout, "test_instructions\n");
    test_memory();
    test_arithmetic();

    printf("PASSED!\n");
    return EXIT_SUCCESS;
}
