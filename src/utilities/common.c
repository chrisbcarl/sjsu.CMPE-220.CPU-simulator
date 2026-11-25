/*
2026-11-21 - Chris Carl - common.c - added indexof, assert
2026-11-22 - Chris Carl - common.c - added strcnt
*/

#include <stddef.h>  // size_t
// #include <stdio.h>
#include "common.h"

char COMMON_BUFFER_0[COMMON_BUFFER_LEN] = {};  // temporary buffer for anyone to use
char COMMON_BUFFER_1[COMMON_BUFFER_LEN] = {};  // temporary buffer for anyone to use

int indexof(char* string, char c) {
    /**
     * @authors Chris Carl
     * Given a string, find the first index of a specific character.
     * https://stackoverflow.com/a/3217634
     *  ex) indexof("hello", 'e') returns 1
     *
     * @param char* - string
     * @param char - the character you're searching for
     * @param int - -1 or string.indexof(c)
     */

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
    /**
     * @authors Chris Carl
     * Count the number of times a character occurs in a string, similar to indexof logic
     *
     * @param char* - string
     * @param char - c
     * @return size_t - the amount of times that character occurred
     */
    size_t count = 0;
    char *ptr = string;
    while (*ptr != '\0') {
        count += (*ptr == c) ? 1 : 0;
        ptr += 1;
    }
    return count;
}
