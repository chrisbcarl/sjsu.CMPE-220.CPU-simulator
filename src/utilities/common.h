/*
common.h is designed to house commonly used utilities that don't really have much to do with the main program.
    assert - macro to help with test cases
    COMMON_BUFFER_0/COMMON_BUFFER_1/COMMON_BUFFER_LEN - in case you just need some character sandbox, like when constructing strings or whatever
    indexof/strcnt - some usual string creature comforts
*/


#ifndef H_COMMMON
#define H_COMMON

#include <stddef.h>  // size_t

#define assert(condition, failure_message, ...) \
    if (!(condition)) {fprintf(stderr, failure_message __VA_OPT__(,) __VA_ARGS__); exit(1);}

#define COMMON_BUFFER_LEN 256
extern char COMMON_BUFFER_0[COMMON_BUFFER_LEN];  // temporary buffer for anyone to use
extern char COMMON_BUFFER_1[COMMON_BUFFER_LEN];  // temporary buffer for anyone to use

int indexof(char*, char);
size_t strcnt(char*, char);


#endif
