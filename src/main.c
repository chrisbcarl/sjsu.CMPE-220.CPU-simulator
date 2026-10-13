/*
make -f CMPE-220/assignments/project/Makefile

*/


// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// project
#include <errors.h>
#include <test-bench.h>



int main(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    die_on_failure(test_bench_gates);
    die_on_failure(test_bench_arithmetic);
    die_on_failure(test_bench_logic);
    die_on_failure(test_bench_registers);
    die_on_failure(test_bench_alu);

    /*
    should be something like
    while true:
    for registered ip block in cpu that depends on clock:
        run that function with the current state of the cpu, clock, registers, outputs, etc.
    clk += 1


    registered functions should include
        each memory cell
        fetch
        decode
        execute
        counters
        alus
    */

    printf("SUCCESS!\n");
    return EXIT_SUCCESS;
}
