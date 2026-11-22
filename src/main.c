/*
make -f CMPE-220/assignments/project/Makefile

2026-11-22 - Chris Carl - main.c - main runs mainloop of cpu cycle, incrementing every time
2026-10-14 18:51 - Chris Carl - main - added test_bench_cpu_simple, worked without compilation or runtime errors first try, kinda strange that it didn't have anything.
*/


// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// project
#include "common.h"
#include "core.h"

bool DEBUG = true;
uint32_t CYCLES = 5;


int main(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    printf("INFO: setup\n");
    core_setup();

    ram[0] = 0x4320BEEF;  // LI R2, "0x0000 BEEF"
    ram[1] = 0x4430DEAD;  // LUI R3, "0xDEAD 0000"
    ram[2] = 0x71123000;  // ADD R1, R2, R3 - "0x0000 BEEF" + "0xDEAD 0000" = "0xDEAD BEEF"

    printf("INFO: running\n");

    int res = cycle(0); uint32_t cycles = 0;
    while (res == 0 && cycles < CYCLES) {
        if (DEBUG) core_dump(cycles);
        clear_busses();
        if (res != 0) fprintf(stderr, "ERROR: failed at step %d, cycle %u!\n", res, cycles);

        res = cycle(cycles); cycles += 1;
    };
    core_dump(cycles);

    printf("INFO: teardown\n");
    core_teardown();
}
