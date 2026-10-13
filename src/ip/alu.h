#ifndef H_ALU
#define H_ALU

// stdlib
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>  // size_t

// project
#include <registers.h>
#include <gates.h>
#include <arithmetic.h>

void adder(uint8_t bits, bool* x, bool* y, bool* s);

#endif
