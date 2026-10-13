/*
run through the gammut of possible functions

2026-10-10 21:02 - test-bench.c - now we've got gates, arithmetic, logic, registers, and alu tested for.
*/

// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>  // size_t

// project
#include <test-bench.h>


int test_bench_gates() {
    printf("--------------------------------------------------------------------------------\n");
    printf("TEST BENCH GATES\n");
    bool x[2] = {0, 1};
    bool y[2] = {0, 1};
    bool z[2] = {0, 1};
    printf("    - AND\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("        x(%d) & y(%d) = %d\n", x[i], y[j], AND(x[i], y[j]));
        }
    }
    printf("    - OR\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("        x(%d) | y(%d) = %d\n", x[i], y[j], OR(x[i], y[j]));
        }
    }
    printf("    - XOR\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("        x(%d) opllus y(%d) = %d\n", x[i], y[j], XOR(x[i], y[j]));
        }
    }
    printf("    - NOT\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("        ^x(%d)       = %d\n", x[i], NOT(x[i]));
        }
    }
    printf("    - ASSIGN\n");
    bool a = 0;
    bool b = 1;
    bool c = 0;
    printf("        -   a=%d, b=%d, c=%d\n", a, b, c);
    ASSIGN(&a, &b);  // a <- 1
    ASSIGN(&b, &c);  // b <- 0
    ASSIGN(&c, &a);  // c <- 1
    printf("        - a=%d, b=%d, c=%d\n", a, b, c);

    printf("    - 3-input gates\n");
    printf("        |x|y|z|and|or|nand|nor|xor|xnor|\n");
    printf("        |-|-|-|---|--|----|---|---|----|\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                    bool a = x[i], b = y[j], c = z[k];
                    bool and = AND3(a, b, c), or = OR3(a, b, c), nand = NAND3(a, b, c), nor = NOR3(a, b, c), xor = XOR3(a, b, c), xnor = XNOR3(a, b, c);
                    printf("        |%d|%d|%d|%d  |%d |%d   |%d  |%d  |%d   |\n", a, b, c, and, or, nand, nor, xor, xnor);
            }
        }
    }

    return EXIT_SUCCESS;
}


int test_bench_arithmetic() {
    printf("--------------------------------------------------------------------------------\n");
    printf("TEST BENCH ARITHMETIC\n");
    bool x0[2] = {0, 1};
    bool y0[2] = {0, 1};
    bool x1[2] = {0, 1};
    bool y1[2] = {0, 1};
    bool c[2] = {0, 0};
    bool s[2] = {0, 0};
    bool sum_arr[8] = {0};

    printf("    - half_adder\n");
    int i = 0, j = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            s[0] = half_adder(x0[i], y0[j], c+j);
            printf("        x(%d) + y(%d) = %d, c=%d\n", x0[i], y0[j], s[0], c[j]);
        }
    }
    printf("    - full_adder\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            bool c_old = c[j];
            s[0] = full_adder(x0[i], y0[j], c+j);
            printf("        x(%d) + y(%d) + c(%d) = %d, c=%d\n", x0[i], y0[j], c_old, s[0], c[j]);
        }
    }
    printf("    - adder_2bit\n");
    for (int i1 = 0; i1 < 2; i1++) {
        for (int i0 = 0; i0 < 2; i0++) {
            for (int j1 = 0; j1 < 2; j1++) {
                for (int j0 = 0; j0 < 2; j0++) {
                    adder_2bit(x1[i1], x0[i0], y1[j1], y0[j0], s + 1, s + 0, c);
                    printf("        x(%d%d) + y(%d%d) = %d%d, c=%d\n", x1[i1], x0[i0], y1[j1], y0[j0], s[1], s[0], c[0]);
                }
            }
        }
    }

    printf("    - adder_nbit\n");
    size_t sum, nums[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    c[0] = 0;
    for (i = 0; i < 8; i += 3) {
        for (j = 0; j < 8; j += 2) {
            sum = adder_nbit(3, nums[i], nums[j], c);
            printf("        x(%02lu) + y(%02lu) = %02lu, c=%d\n", nums[i], nums[j], sum, c[0]);
        }
    }

    printf("    - adder_8bit\n");
    adder_8bit(
        1,0,0,0,0,0,0,1,  // x
        1,0,0,0,0,0,1,1,  // y
        sum_arr+7, sum_arr+6, sum_arr+5, sum_arr+4, sum_arr+3, sum_arr+2, sum_arr+1, sum_arr+0,
        c);
    printf("        x(0b10000001) + y(0b10000011) = 0b%d%d%d%d%d%d%d%d, c=%d\n", sum_arr[7], sum_arr[6], sum_arr[5], sum_arr[4], sum_arr[3], sum_arr[2], sum_arr[1], sum_arr[0], c[0]);
    adder_8bit(
        1,1,1,1,0,0,0,0,  // x
        0,0,0,0,1,1,1,1,  // y
        sum_arr+7, sum_arr+6, sum_arr+5, sum_arr+4, sum_arr+3, sum_arr+2, sum_arr+1, sum_arr+0,
        c);
    printf("        x(0b11110000) + y(0b00001111) = 0b%d%d%d%d%d%d%d%d, c=%d\n", sum_arr[7], sum_arr[6], sum_arr[5], sum_arr[4], sum_arr[3], sum_arr[2], sum_arr[1], sum_arr[0], c[0]);

    return EXIT_SUCCESS;
}

int test_bench_logic() {
    printf("--------------------------------------------------------------------------------\n");
    printf("TEST BENCH LOGIC\n");

    bool t = 0, clk = 1, s = 0, r = 0, q = 0, q_not = 1, q_prev=0, q_not_prev=1;
    q_prev = q, q_not_prev = q_not;

    /* ******************************************************************* */
    printf("    - sr_flip_flop\n");
    // hold
    clk = 1; s = 0, r = 0; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // set, bad clock
    clk = 0; s = 1, r = 0; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | SET, bad clock, aka HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // set
    clk = 1; s = 1, r = 0; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | SET\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // hold
    clk = 1; s = 0, r = 0; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // reset, bad clock
    clk = 0; s = 0, r = 1; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | RESET, bad clock, aka HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // reset
    clk = 1; s = 0, r = 1; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | RESET\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // hold
    clk = 1; s = 0, r = 0; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // contradiction, bad clk
    clk = 0; s = 1, r = 1; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | CONTRADICTION, bad clock, aka HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // contradiction
    clk = 1; s = 1, r = 1; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | CONTRADICTION\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // contradiction
    clk = 1; s = 1, r = 1; q_prev = q, q_not_prev = q_not; sr_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | CONTRADICTION\n", clk, q_prev, q_not_prev, s, r, q, q_not);

    /* ******************************************************************* */
    printf("    - jk_flip_flop\n");
    // hold
    clk = 1; s = 0, r = 0; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // set, bad clock
    clk = 0; s = 1, r = 0; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | SET, bad clock, aka HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // set
    clk = 1; s = 1, r = 0; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | SET\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // hold
    clk = 1; s = 0, r = 0; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // reset, bad clock
    clk = 0; s = 0, r = 1; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | RESET, bad clock, aka HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // reset
    clk = 1; s = 0, r = 1; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | RESET\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // hold
    clk = 1; s = 0, r = 0; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // toggle, bad clk
    clk = 0; s = 1, r = 1; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | TOGGLE, bad clock, aka HOLD\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // toggle
    clk = 1; s = 1, r = 1; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | TOGGLE\n", clk, q_prev, q_not_prev, s, r, q, q_not);
    // toggle
    clk = 1; s = 1, r = 1; q_prev = q, q_not_prev = q_not; jk_flip_flop(clk, s, r, &q, &q_not);
    printf("        clk=%d, q=%d, !q=%d, s=%d, r=%d -> q`=%d, !q`=%d | TOGGLE\n", clk, q_prev, q_not_prev, s, r, q, q_not);

    /* ******************************************************************* */
    printf("    - t_flip_flop\n");
    // hold
    clk = 1; t = 0, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // toggle, bad clock
    clk = 0; t = 1, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | TOGGLE, bad clock, aka HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // toggle
    clk = 1; t = 1, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | TOGGLE\n", q_prev, q_not_prev, clk, t, q, q_not);
    // hold
    clk = 1; t = 0, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // hold, bad clock
    clk = 0; t = 0, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | HOLD, bad clock, aka HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // toggle
    clk = 1; t = 1, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | TOGGLE\n", q_prev, q_not_prev, clk, t, q, q_not);
    // hold
    clk = 1; t = 0, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // toggle, bad clk
    clk = 0; t = 1, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | TOGGLE, bad clock, aka HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // hold
    clk = 1; t = 0, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | HOLD\n", q_prev, q_not_prev, clk, t, q, q_not);
    // toggle
    clk = 1; t = 1, q_prev = q, q_not_prev = q_not; t_flip_flop(clk, t, &q, &q_not);
    printf("        q=%d, !q=%d, clk=%d, t=%d -> q`=%d, !q`=%d | TOGGLE\n", q_prev, q_not_prev, clk, t, q, q_not);

    printf("    - counter_1bit\n");
    counter_1bit(0, &clk);
    counter_1bit(1, &clk);

    printf("    - counter_2bit\n");
    counter_2bit(0, 0, &clk);
    counter_2bit(1, 0, &clk);

    printf("    - counter_3bit\n");
    counter_3bit(0, 0, 0, &clk);
    counter_3bit(0, 1, 1, &clk);
    counter_3bit(1, 1, 0, &clk);

    printf("    - counter_4bit\n");
    counter_4bit(0, 0, 0, 0, &clk);
    counter_4bit(0, 0, 1, 1, &clk);
    counter_4bit(1, 0, 0, 1, &clk);


    return EXIT_SUCCESS;
}


int test_bench_registers() {
    printf("--------------------------------------------------------------------------------\n");
    printf("TEST BENCH REGISTERS\n");

    printf("    - SET, RESET, TEST\n");

    printf("        carry bit %d\n", FLAGS.carry);
    SET(carry);
    printf("        carry bit %d after SET(carry),   TEST(carry) == %d\n", FLAGS.carry, TEST(carry));
    RESET(carry);
    printf("        carry bit %d after RESET(carry), TEST(carry) == %d\n", FLAGS.carry, TEST(carry));

    return EXIT_SUCCESS;
}


int test_bench_alu() {
    printf("--------------------------------------------------------------------------------\n");
    printf("TEST BENCH ALU\n");

    bool X[8] = {1,0,0,0,0,0,1,1};  // big endian 0x10000011
    bool Y[8] = {1,0,0,0,0,0,0,1};
    bool S[8] = {0};

    printf("    - adder:\n");
    adder(8, X, Y, S);
    printf("        x(0b%d%d%d%d%d%d%d%d) + y(0b%d%d%d%d%d%d%d%d) = 0b%d%d%d%d%d%d%d%d, c=%d\n",
        X[0], X[1], X[2], X[3], X[4], X[5], X[6], X[7],
        Y[0], Y[1], Y[2], Y[3], Y[4], Y[5], Y[6], Y[7],
        S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7],
        FLAGS.carry);
    bool X2[8] = {1,0,0,1,0,0,0,0};  // big endian 0x10010000
    bool Y2[8] = {1,0,0,0,0,0,0,0};
    adder(8, X2, Y2, S);
    printf("        x(0b%d%d%d%d%d%d%d%d) + y(0b%d%d%d%d%d%d%d%d) = 0b%d%d%d%d%d%d%d%d, c=%d\n",
        X2[0], X2[1], X2[2], X2[3], X2[4], X2[5], X2[6], X2[7],
        Y2[0], Y2[1], Y2[2], Y2[3], Y2[4], Y2[5], Y2[6], Y2[7],
        S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7],
        FLAGS.carry);


    return EXIT_SUCCESS;
}