/*
make -f CMPE-220/assignments/project/Makefile

2026-10-14 18:51 - main - chriscarl - added test_bench_cpu_simple, worked without compilation or runtime errors first try, kinda strange that it didn't have anything.
*/


// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// project
#include <errors.h>
#include <bench.h>



int main(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

}
