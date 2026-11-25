/*
2026-11-23 - Chris Carl - test_programs.c - added LI, LUI, ADD and test scaffolding
2026-11-25 - Chris Carl - test_programs.c - added ADD variants to test flag generation, all is well

Build and run from the repository root:
    mkdir -p bin
    cc -std=c11 -Wall -Wextra -I./src/utilities src/utilities/common.c -Isrc/cpu src/cpu/isa.c -I./src/cpu src/cpu/core.c test/isa/test_programs.c -o bin/test_programs
    ./bin/test_isa
*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"  // COMMON_BUFFER_0, assert
#include "isa.h"
#include "core.h"

typedef struct test_program {
    const char* program_name;
    bool dump_final_clock;
    bool dump_every_clock;
    size_t program_length;
    uint8_t flags_init;
    uint8_t flags_expected;
    const uint32_t* gprs_init;
    const uint32_t* gprs_expected;
    const uint32_t* program;
} test_program;

// reclaim some space, otherwise declaring an array of test_programs is quite verbose
typedef const uint32_t cua_t[];  // const uint32_t[]
#define REG_INIT [0x0]=0,[0x1]=0,[0x2]=0,[0x3]=0,[0x4]=0,[0x5]=0,[0x6]=0,[0x7]=0,[0x8]=0,[0x9]=0,[0xa]=0,[0xb]=0,[0xc]=0,[0xd]=0,[0xe]=0,[0xf]=0
#define INSTR_INIT [0x0]=0,[0x1]=0,[0x2]=0,[0x3]=0,[0x4]=0,[0x5]=0,[0x6]=0,[0x7]=0,[0x8]=0,[0x9]=0,[0xa]=0,[0xb]=0,[0xc]=0,[0xd]=0,[0xe]=0,[0xf]=0
#define nZnCnO  ( (!FLAG_Z) | (!FLAG_C) | (!FLAG_O) )
#define nZnCO   ( (!FLAG_Z) | (!FLAG_C) |   FLAG_O  )
#define nZCnO   ( (!FLAG_Z) |   FLAG_C  | (!FLAG_O) )
#define nZCO    ( (!FLAG_Z) |   FLAG_C  |   FLAG_O  )
#define ZnCnO   (   FLAG_Z  | (!FLAG_C) | (!FLAG_O) )
#define ZnCO    (   FLAG_Z  | (!FLAG_C) |   FLAG_O  )
#define ZCnO    (   FLAG_Z  |   FLAG_C  | (!FLAG_O) )
#define ZCO     (   FLAG_Z  |   FLAG_C  |   FLAG_O  )

#pragma GCC diagnostic ignored "-Woverride-init"

void load_run_test(const test_program* tp) {
    // reset
    core_reset();  // better to simply reset than to load and unload the core all the time
    // load
    for (size_t i = 0; i < tp->program_length; i++) {RAM[i] = tp->program[i];}
    // init
    set_flags(tp->flags_init);
    for (size_t i = 0; i < ISA_NUM_GPRS; i++) {
        set_register_value(i, tp->gprs_init[i]);
    }
    // run
    int res = core_run(tp->program_length, tp->dump_final_clock, tp->dump_every_clock, true, 3);
    assert(res == EXIT_SUCCESS, "'%s' failed to run!\n", tp->program_name);
    // test
    for (size_t i = 0; i < ISA_NUM_GPRS; i++) {
        uint32_t expected = tp->gprs_expected[i];
        if (expected == 0) continue;  // uninteresting or rather the rest have been uninitialized
        uint32_t value = get_register_value(i);
        if (value != 0) assert(value == expected, "'%s' R%lu expected 0x%x, encountered 0x%x!\n", tp->program_name, i, expected, value);
    }
    assert(get_flags() == tp->flags_expected, "'%s' FLAGS expected 0x%x, encountered 0x%x!\n", tp->program_name, tp->flags_expected, get_flags());
}

int main(void) {
    /**
     * @author Chris Carl
     * The intention of this is, write VERY SMALL test programs that utilize all of the instructions you've just added.
     *  It's also a way to test flag results at the end of these programs.
     */
    fprintf(stdout, "test_programs\n");

    // TODO: add new test programs here!
    const test_program tps[] = {
        // Chris Carl's very first program ever. It's a miracle how all of this just works...
        {
            "LI-LUI-ADD", true, false, 3, nZnCnO, nZnCnO,  // name, core dump on final clock, core dump each clock, instruction count, init flags, expected flags
            (cua_t){  // gpr init
                REG_INIT,  // fills in [0xi] = 0
            },
            (cua_t){  // gpr expected
                REG_INIT,  // [0xi] = 0, overriden just below
                [1] = 0xDEADBEEF,  // R1
                [2] = 0x0000BEEF,  // R2
                [3] = 0xDEAD0000,  // R3
            },
            (cua_t){  // binary instructions
                INSTR_INIT,  // [0xi] = 0, overriden just below
                [0] = 0x4320BEEF,  // LI R2, "0x0000 BEEF"
                [1] = 0x4430DEAD,  // LUI R3, "0xDEAD 0000"
                [2] = 0x71123000,  // ADD R1, R2, R3 - "0x0000 BEEF" + "0xDEAD 0000" = "0xDEAD BEEF"
            },
        },

        // LI/LUI
        {"LI R2, 0xBEEF", false, false, 3, nZnCnO, nZnCnO,
            (cua_t){REG_INIT}, (cua_t){REG_INIT, [2] = 0x0000BEEF},
            (cua_t){INSTR_INIT, [0] = 0x4320BEEF},},
        {"LUI R3, 0xDEAD", false, false, 3, nZnCnO, nZnCnO,
            (cua_t){REG_INIT}, (cua_t){REG_INIT, [3] = 0xDEAD0000},
            (cua_t){INSTR_INIT, [0] = 0x4430DEAD},},

        // ADD R3, R1, R2 - normal, no flags
        {   "ADD(1+2)", false, false, 1, nZnCnO, nZnCnO,
            (cua_t){REG_INIT, [1]=0x1, [2]=0x2}, (cua_t){REG_INIT, [3]=0x3},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(1+2)-swap", false, false, 1, nZnCnO, nZnCnO,
            (cua_t){REG_INIT, [2]=0x1, [1]=0x2}, (cua_t){REG_INIT, [3]=0x3},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},

        // ADD R3, R1, R2 - create 0, expect ZO
        {   "ADD(1+-1)", false, false, 1, nZnCnO, ZCnO,
            (cua_t){REG_INIT, [1]=0x1, [2]=0xFFFFFFFF}, (cua_t){REG_INIT, [3]=0x0},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(1+-1)-swap", false, false, 1, nZnCnO, ZCnO,
            (cua_t){REG_INIT, [2]=0x1, [1]=0xFFFFFFFF}, (cua_t){REG_INIT, [3]=0x0},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(-1+1)", false, false, 1, nZnCnO, ZCnO,
            (cua_t){REG_INIT, [1]=0xFFFFFFFF, [2]=0x1}, (cua_t){REG_INIT, [3]=0x0},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(-1+1)-swap", false, false, 1, nZnCnO, ZCnO,
            (cua_t){REG_INIT, [2]=0xFFFFFFFF, [1]=0x1}, (cua_t){REG_INIT, [3]=0x0},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},

        // ADD R3, R1, R2 - 2 positive, go negative, expect O
        {   "ADD(0x7F+1)", false, false, 1, nZnCnO, nZnCO,
            (cua_t){REG_INIT, [1]=0x7FFFFFFF, [2]=0x1}, (cua_t){REG_INIT, [3]=0x80000000},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(0x7F+1)-swap", false, false, 1, nZnCnO, nZnCO,
            (cua_t){REG_INIT, [2]=0x7FFFFFFF, [1]=0x1}, (cua_t){REG_INIT, [3]=0x80000000},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(0x7F+0x7F)", false, false, 1, nZnCnO, nZnCO,
            (cua_t){REG_INIT, [1]=0x7FFFFFFF, [2]=0x7FFFFFFF}, (cua_t){REG_INIT, [3]=0xFFFFFFFE},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},

        // ADD R3, R1, R2 - 1 negative, 1 positive, go positive, expect C
        {   "ADD(-1+2)", false, false, 1, nZnCnO, nZCnO,
            (cua_t){REG_INIT, [1]=0xFFFFFFFF, [2]=0x2}, (cua_t){REG_INIT, [3]=0x1},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(-1+2)-swap", false, false, 1, nZnCnO, nZCnO,
            (cua_t){REG_INIT, [2]=0xFFFFFFFF, [1]=0x2}, (cua_t){REG_INIT, [3]=0x1},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},

        // ADD R3, R1, R2 - 1 negative, 1 positive, go negative, expect nothing
        {   "ADD(-2+1)", false, false, 1, nZnCnO, nZnCnO,
            (cua_t){REG_INIT, [1]=0xFFFFFFFE, [2]=0x1}, (cua_t){REG_INIT, [3]=0xFFFFFFFF},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
        {   "ADD(-2+1)-swap", false, false, 1, nZnCnO, nZnCnO,
            (cua_t){REG_INIT, [2]=0xFFFFFFFE, [1]=0x1}, (cua_t){REG_INIT, [3]=0xFFFFFFFF},
            (cua_t){INSTR_INIT, [0] = 0x71312000,}},
    };

    // setup
    core_setup();
    size_t TEST_PROGRAM_COUNT = sizeof(tps) / sizeof(tps[0]);
    for (size_t i = 0; i < TEST_PROGRAM_COUNT; i++) {
        fprintf(stdout, "============ (%lu/%lu) - '%s' ============\n", i + 1, TEST_PROGRAM_COUNT, (tps + i)->program_name);
        load_run_test(tps + i);
        fprintf(stdout, "    PASSED!\n");
    }
    // teardown
    core_teardown();

    printf("PASSED!\n");
    return EXIT_SUCCESS;
}
