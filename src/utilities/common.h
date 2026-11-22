#ifndef H_COMMMON
#define H_COMMON

#include <stddef.h>

#define BUFLEN 256
extern char BUFFER0[BUFLEN];  // temporary buffer for anyone to use
extern char BUFFER1[BUFLEN];  // temporary buffer for anyone to use

int indexof(char*, char);
size_t strcnt(char*, char);


#endif
