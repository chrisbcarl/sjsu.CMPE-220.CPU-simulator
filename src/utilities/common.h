#ifndef H_COMMMON
#define H_COMMON

#include <stddef.h>

#define assert(condition, failure_message, ...) \
    if (!(condition)) {fprintf(stderr, failure_message __VA_OPT__(,) __VA_ARGS__); exit(1);}

#define BUFLEN 256
extern char BUFFER0[BUFLEN];  // temporary buffer for anyone to use
extern char BUFFER1[BUFLEN];  // temporary buffer for anyone to use

int indexof(char*, char);
size_t strcnt(char*, char);


#endif
