/*
make -f CMPE-220/assignments/project/Makefile

2026-11-22 - Chris Carl - main.c - main runs mainloop of cpu cycle, incrementing every time
2026-10-14 18:51 - Chris Carl - main.c - added test_bench_cpu_simple, worked without compilation or runtime errors first try, kinda strange that it didn't have anything.
*/


// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// project
#include "common.h"
#include "core.h"

bool DEBUG;
bool BREAK_ON_HALT;
uint32_t CLOCKS;
uint32_t ADDRESSES_IN_DUMP;

int main(int argc, char** argv) {
    DEBUG = true;
    BREAK_ON_HALT = true;
    CLOCKS = 5;
    ADDRESSES_IN_DUMP = 5;
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
        // TODO: getopt here, -c clocks -a file.isa -b file.bin, -i interactive, etc.
    }

    printf("INFO: setup\n");
    core_setup();

    // TODO: how do we load programs? this is just hardcoded...
    ram[0] = 0x4320BEEF;  // LI R2, "0x0000 BEEF"
    ram[1] = 0x4430DEAD;  // LUI R3, "0xDEAD 0000"
    ram[2] = 0x71123000;  // ADD R1, R2, R3 - "0x0000 BEEF" + "0xDEAD 0000" = "0xDEAD BEEF"

    printf("INFO: running\n");
    run(CLOCKS, DEBUG, BREAK_ON_HALT, ADDRESSES_IN_DUMP);

    printf("INFO: teardown\n");
    core_teardown();
}
