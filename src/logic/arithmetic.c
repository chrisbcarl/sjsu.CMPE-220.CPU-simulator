/*
gcc CMPE-220/assignments/project/fundamental.c -o dist/main.exe; dist/main.exe
implement the arithmetic from atomic gates, kinda an ALU situation

2026-10-10 18:10 - arithmetic.c - removed the counters and flip flops, doesnt make sense here.
2026-10-10 14:59 - arithmetic.c - start subtract and counters, realizing I need globals for clock and carry rather than something else...
2026-10-09 18:40 - arithmetic.c - adders finished, counters remain
2026-10-09 16:28 - arithmetic.c - start half adder, full adder, 8-bit adder, jk flip flop, t flip flop, counter, 3-bit counter, 4-bit counter
*/

// stdlib
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>  // size_t
#include <stdio.h>

// project
#include <gates.h>

bool half_adder(bool x, bool y, bool* c) {
    /**
     * @param bool x - input
     * @param bool y - input
     * @param bool* c - output carry bit
     * @return bool - sum of x + y
     *
     * Implemented via XOR and AND
     * |x|y|s=x+y|c|
     * |-|-|-----|-|
     * |0|0|0    |0|
     * |0|1|1    |0|
     * |1|0|1    |0|
     * |1|1|0    |1|
     * they look suspicously like XOR and AND... so just use them.
     *
     * USAGE:
     *  - clear a variable: x <- x XOR x
     *  - half-adder: x <- x XOR x
     */
    *c = AND(x, y);
    return XOR(x, y);
}

bool full_adder(bool x, bool y, bool* c) {
    /**
     * @param bool x - input
     * @param bool y - input
     * @param bool* c - output carry bit
     * @return bool - sum of x + y + c
     *
     * Full adder is cannonically "two halves stitched together, OR the carries"
     *  Read the carry like a most significant bit
     * |c|x|y|s=x+y+c|c|cs|
     * |-|-|-|-------|-|--|
     * |0|0|0|0      |0|00|
     * |0|0|1|1      |0|01|
     * |0|1|0|1      |0|01|
     * |0|1|1|0      |1|10|
     * |1|0|0|1      |0|01|
     * |1|0|1|0      |1|10|
     * |1|1|0|0      |1|10|
     * |1|1|1|1      |1|11|
     */
    bool s0 = false; bool c0 = false;
    bool s1 = false; bool c1 = false;
    s0 = half_adder(x, y, &c0);  // add the x and y, get s0, c0
    s1 = half_adder(s0, *c, &c1);  // add the s0 and c_in, get s1, c1
    *c = OR(c0, c1);
    return s1;
}

void adder_2bit(
    bool  x1, bool  x0,
    bool  y1, bool  y0,
    bool* s1, bool* s0,
    bool* c
) {
    /**
     * @param bool x0 - input
     * @param bool x1 - input
     * @param bool y0 - input
     * @param bool y1 - input
     * @param bool s0 - output
     * @param bool s1 - output
     * @param bool* c - output carry bit
     * @return bool - sum of x + y + c
     * n-adder is cannonically stitch a half adder to a string of full adders
     */
    *s0 = half_adder(x0, y0, c);   // carry gets modified in progress
    *s1 = full_adder(x1, y1, c);   // carry is returned modified anyway
}

void adder_4bit(
    bool  x3, bool  x2, bool  x1, bool  x0,
    bool  y3, bool  y2, bool  y1, bool  y0,
    bool* s3, bool* s2, bool* s1, bool* s0,
    bool* c
) {
    *c = 0;
    *s0 = full_adder(x0, y0, c);
    *s1 = full_adder(x1, y1, c);
    *s2 = full_adder(x2, y2, c);
    *s3 = full_adder(x3, y3, c);
}

void adder_8bit(
    bool  x7, bool  x6, bool  x5, bool  x4, bool  x3, bool  x2, bool  x1, bool  x0,
    bool  y7, bool  y6, bool  y5, bool  y4, bool  y3, bool  y2, bool  y1, bool  y0,
    bool* s7, bool* s6, bool* s5, bool* s4, bool* s3, bool* s2, bool* s1, bool* s0,
    bool* c
) {
    adder_4bit(x3, x2, x1, x0, y3, y2, y1, y0, s3, s2, s1, s0, c);
    adder_4bit(x7, x6, x5, x4, y7, y6, y5, y4, s7, s6, s5, s4, c);
}

size_t adder_nbit(size_t bits, size_t x, size_t y, bool* c) {
    /**
     * using primitive logic combined with actual numbers is interesting, so...
     * @param size_t bits - n-bit adder
     * @param size_t x - input
     * @param size_t y - input
     * @param bool* c - output carry
     * @return size_t - SUM
     */
    size_t acc = 0;
    bool xt = (bool)((x >> 0) & 0x1), yt = (bool)((y >> 0) & 0x1);
    bool s = half_adder(xt, yt, c);
    acc |= (size_t)s;

    for (size_t i = 1; i < bits; i++) {
        xt = (bool)((x >> i) & 0x1), yt = (bool)((y >> i) & 0x1);
        s = full_adder(xt, yt, c);
        acc |= ((size_t) s << i);
    }
    return acc;
}