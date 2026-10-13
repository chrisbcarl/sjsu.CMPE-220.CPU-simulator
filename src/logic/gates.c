/*
gcc CMPE-220/assignments/project/fundamental.c -o dist/main.exe; dist/main.exe
implement the boolean atomic functions as 1-bit arguments

2026-10-10 21:02 - logic.c - added 3-input variants to support the latch logics
2026-10-09 16:28 - gates.c - added NAND, NOR, XNOR
2026-10-09 16:19 - gates.c - start NAND, NOR, XNOR
*/

#include <stdint.h>
#include <stdbool.h>

bool AND(bool x, bool y) {
    return x & y;
}

bool OR(bool x, bool y) {
    return x | y;
}

bool NOT(bool x) {
    return x ^ 1;
}

bool NAND(bool x, bool y) {
    return NOT(AND(x, y));
}

bool NOR(bool x, bool y) {
    return NOT(OR(x, y));
}

bool XOR(bool x, bool y) {
    /**
     * Implemented via Disjunctive Normal Form (DNF, disjunction of conjunctions) of XOR
     * |x|y|xor|CF    |
     * |-|-|---|------|
     * |0|0|0  |      |
     * |0|1|1  |!x & y|
     * |1|0|1  |x & !y|
     * |1|1|0  |      |
     * DNF(XOR) = (!x & y) | (x & !y)
     *
     * USAGE:
     *  - clear a variable: x <- x XOR x
     *  - half-adder: x <- x XOR x
     */
    return OR(AND(NOT(x), y), AND(x, NOT(y)));  // DNF
}

bool XNOR(bool x, bool y) {
    return NOT(XOR(x, y));
}

void ASSIGN(bool* x, bool* y) {
    /**
     * x = y
     *  1. clear x (via XOR), x = 0
     *  2. x <- x(0) or y
     */
    *x = XOR(*x, *x);
    *x = OR(*x, *y);
}

bool AND3(bool x, bool y, bool z) {
    return AND(AND(x, y), z);
}

bool OR3(bool x, bool y, bool z) {
    return OR(OR(x, y), z);
}

bool NAND3(bool x, bool y, bool z) {
    return NOT(AND3(x, y, z));
}

bool NOR3(bool x, bool y, bool z) {
    return NOT(OR3(x, y, z));
}

bool XOR3(bool x, bool y, bool z) {
    return XOR(XOR(x, y), z);
}

bool XNOR3(bool x, bool y, bool z) {
    return NOT(XOR3(x, y, z));
}
