/*
 * 2026-11-23 - Chris Carl - test_programs.c - added LI, LUI, ADD and test scaffolding
 *
 * Build and run from the repository root:
 * mkdir -p bin
 * cc -std=c11 -Wall -Wextra -I./src/utilities src/utilities/common.c -Isrc/cpu src/cpu/isa.c -I./src/cpu src/cpu/core.c src/cpu/instructions.c test/isa/test_programs.c -o bin/test_programs
 * ./bin/test_isa
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"  // BUFFER0, assert
#include "isa.h"
#include "core.h"

typedef struct test_program {
    const char* program_name;
    const uint32_t* program;
    size_t program_length;
    const uint32_t* gprs;
    uint8_t flags;
    bool debug;
} test_program;

#define TEST_PROGRAM_COUNT 1
const test_program test_programs[TEST_PROGRAM_COUNT] = {
    {
        "LI-LUI-ADD",
        (const uint32_t[]){
            [0] = 0x4320BEEF,  // LI R2, "0x0000 BEEF"
            [1] = 0x4430DEAD,  // LUI R3, "0xDEAD 0000"
            [2] = 0x71123000,  // ADD R1, R2, R3 - "0x0000 BEEF" + "0xDEAD 0000" = "0xDEAD BEEF"
        },
        3,
        (const uint32_t[]){
            [1] = 0xDEADBEEF,  // R1
            [2] = 0x0000BEEF,  // R2
            [3] = 0xDEAD0000,  // R3
        },
        0,
        false
    }
};

void load_run_test(const test_program* tp) {
    core_setup();
    // load
    for (size_t i = 0; i < tp->program_length; i++) {ram[i] = tp->program[i];}
    // run
    int res = run(tp->program_length, tp->debug);
    assert(res == EXIT_SUCCESS, "'%s' failed to run!\n", tp->program_name);
    // test
    for (size_t i = 0; i < ISA_NUM_GPRS; i++) {
        uint32_t value = get_register_value(i);
        uint32_t expected = tp->gprs[i];
        if (value != 0) assert(value == expected, "'%s' R%lu expected 0x%x, encountered 0x%x!\n", tp->program_name, i, expected, value);
    }
    assert(get_flags() == tp->flags, "'%s' FLAGS expected 0x%x, encountered 0x%x!\n", tp->program_name, tp->flags, get_flags());
    core_teardown();
}

int main(void) {
    fprintf(stdout, "test_programs\n");
    for (size_t i = 0; i < TEST_PROGRAM_COUNT; i++) {
        load_run_test(test_programs + i);
    }

    printf("PASSED!\n");
    return EXIT_SUCCESS;
}
