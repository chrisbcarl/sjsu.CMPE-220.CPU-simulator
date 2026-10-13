#ifndef H_ARITHMETIC
#define H_ARITHMETIC

// stdlib
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>  // size_t

// project
#include <gates.h>

bool half_adder(bool x, bool y, bool* c);
bool full_adder(bool x, bool y, bool* c);
void adder_2bit(
    bool  x1, bool  x0,
    bool  y1, bool  y0,
    bool* s1, bool* s0,
    bool* c
);
void adder_4bit(
    bool  x3, bool  x2, bool  x1, bool  x0,
    bool  y3, bool  y2, bool  y1, bool  y0,
    bool* s3, bool* s2, bool* s1, bool* s0,
    bool* c
);
void adder_8bit(
    bool  x7, bool  x6, bool  x5, bool  x4, bool  x3, bool  x2, bool  x1, bool  x0,
    bool  y7, bool  y6, bool  y5, bool  y4, bool  y3, bool  y2, bool  y1, bool  y0,
    bool* s7, bool* s6, bool* s5, bool* s4, bool* s3, bool* s2, bool* s1, bool* s0,
    bool* c
);
size_t adder_nbit(size_t bits, size_t x, size_t y, bool* c);

#endif
