/*
gcc CMPE-220/assignments/project/fundamental.c -o dist/main.exe; dist/main.exe
implement the ALU from "logic" which comes from atomic gates
TODO: just realized multiply requires 2*8 = 16 bits so theres an lsb portion and msb portion, can't really derive that yet.

2026-10-10 17:36 - alu.c - necessity to separate the "logic" from the implementation. Implementation needs globals, logic doesnt. Keep separate.
*/

// stdlib
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>  // size_t
#include <stdio.h>

// project
#include <registers.h>
#include <alu.h>
#include <logging.h>

void adder(uint8_t bits, bool* x, bool* y, bool* s) {
    /**
     * BIG ENDIAN adder implemented using registers and flags and such
     *  x[4] = {1,0,0,0} is 0x1000... MSB in lowest address
     * already works with 2's compliment for subtraction
     *
     * @param uint8_t bits
     * @param bool* x - input big endian array of size bits
     * @param bool* y - input big endian array of size bits
     * @param bool* s - output big endian array of size bits
     */
    s[bits - 1] = half_adder(x[bits - 1], y[bits - 1], &FLAGS.carry);

    for (int i = bits - 2; i > -1; i--) {  // can't use uint since it needs to decrement to -1
        s[i] = full_adder(x[i], y[i], &FLAGS.carry);
    }
}
