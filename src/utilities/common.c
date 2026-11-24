/*
2026-11-21 - Chris Carl - added indexof
2026-11-22 - Chris Carl - added strcnt
*/

#include <stddef.h>
#include <stdio.h>
#include "common.h"

char BUFFER0[BUFLEN] = {};  // temporary buffer for anyone to use
char BUFFER1[BUFLEN] = {};  // temporary buffer for anyone to use

int indexof(char* string, char c) {
    // https://stackoverflow.com/a/3217634
    // string.indexof(c)  == -1 or idx
    char *ptr = string;
    // printf("string 0x%p, ptr 0x%p, ptr: '%c'\n", string, ptr, *ptr);
    while (*ptr != c && *ptr != '\0') {
        ptr += 1;
        // printf("string 0x%p, ptr 0x%p, ptr: '%c'\n", string, ptr, *ptr);
    }
    int idx;
    if (*ptr == '\0') idx = -1;
    else idx = (int)(ptr - string);  // number of bytes between the ptr and the start of the string
    // printf("'%s'.indexof('%c') == %d\n", string, c, idx);
    return idx;
}

size_t strcnt(char* string, char c) {
    size_t count = 0;
    char *ptr = string;
    while (*ptr != '\0') {
        count += (*ptr == c) ? 1 : 0;
        ptr += 1;
    }
    return count;
}
