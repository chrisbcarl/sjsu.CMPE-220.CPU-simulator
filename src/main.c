/*
make -f CMPE-220/assignments/project/Makefile

2026-11-22 - Chris Carl - main.c - main runs mainloop of cpu cycle, incrementing every time
2026-10-14 18:51 - Chris Carl - main.c - added test_bench_cpu_simple, worked without compilation or runtime errors first try, kinda strange that it didn't have anything.

```bash
gcc -g \
    -I./src/bench -I./src/cpu -I./src/ip -I./src/logic -I./src/meta -I./src/utilities \
    ./src/cpu/core.c ./src/cpu/isa.c ./src/ip/alu.c ./src/ip/memory.c \
    ./src/ip/registers.c ./src/logic/arithmetic.c ./src/logic/gates.c \
    ./src/logic/logic.c ./src/meta/errors.c ./src/meta/logging.c ./src/utilities/common.c \
    ./src/main.c \
    -fdiagnostics-color=always -g -o \
    ./bin/main && ./bin/main
*/


// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// project
#include "common.h"
#include "core.h"

bool DUMP_FINAL_CLOCK = true;
bool DUMP_EVERY_CLOCK = false;
bool BREAK_ON_HALT = true;
uint32_t CLOCKS = RAMSIZE;
uint32_t ADDRESSES_IN_DUMP = 5;

int main(int argc, char** argv) {
    /**
     * @authors Chris Carl
     * Entrypoint into the simulator itself. Will mostly contain user-interaction code for setting up simulation behavior.
     *
     * @param int argc - CLI argument count
     * @param char** argv - double array of strings, the compiled program name is always argv[0]
     * @return int - EXIT_SUCCESS or EXIT_FAILURE
     */

    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
        // TODO: getopt here, -c clocks -a file.isa -b file.bin, -i interactive, etc.
        //  modify the globals here
    }

    printf("INFO: setup\n");
    core_setup();

    // TODO: how do we load programs? this is just hardcoded...
    RAM[0] = 0x4320BEEF;  // LI R2, "0x0000 BEEF"
    RAM[1] = 0x4430DEAD;  // LUI R3, "0xDEAD 0000"
    RAM[2] = 0x71123000;  // ADD R1, R2, R3 - "0x0000 BEEF" + "0xDEAD 0000" = "0xDEAD BEEF"
    // NOTE: the rest of the RAM is 0's or HALTs... so must break on halt for now

    printf("INFO: running\n");
    core_run(CLOCKS, DUMP_FINAL_CLOCK, DUMP_EVERY_CLOCK, BREAK_ON_HALT, ADDRESSES_IN_DUMP);

    printf("INFO: teardown\n");
    core_teardown();
}
