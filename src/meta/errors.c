// stdlib
#include <stdlib.h>
#include <stdio.h>

#include <errors.h>

void die(char* msg) {
    fprintf(stderr, "FAIL! %s\n", msg);
    exit(EXIT_FAILURE);
}
