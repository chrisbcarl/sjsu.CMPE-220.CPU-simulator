/*
gcc CMPE-220/assignments/project/fundamental.c -o dist/main.exe; dist/main.exe
implement the arithmetic from atomic gates, kinda an ALU situation
TODO: I don't understand... the counters are behaving like DOWN counters...

2026-10-11 20:18 - logic.c - counters work to some extent, but work as down counters, its weird.
2026-10-11 18:49 - logic.c - begin work on counters
2026-10-10 21:02 - logic.c - once the latches were bootstrapped to sr, jk and t fell like dominos.
2026-10-10 18:10 - logic.c - added to store the flip flops and counter implementations
*/

// stdlib
#include <stdbool.h>
#include <stdio.h>

// project
#include <logging.h>
#include <gates.h>
#include <logic.h>

void sr_latch(bool s, bool r, bool* q, bool* q_not) {
    /**
     * @param bool s - input
     * @param bool bool r - input
     * @param bool bool* q - input/output (prior latched value)
     * @param bool bool* q_not - input/output (prior latched inverse value)
     * Cannonically its the first latch
     *  - 2 NOR (NAND implementable but then you do !S !R)
     *      - !(S|Q)=!Q
     *      - !(R|!Q)=Q
     *  - sequential, depends on previous value of q
     *      - HAS A RACE CONDITION (S, R = 1)
     *      - READING FROM WHO SET 1 FIRST...
     *          - if S is set first, then read the top operation first
     *          - if R is set first, then read the 2nd operation first...
     *  - assume Q initialized to 0
     *      |Q|!Q|S|R|first           |calc       |second          |calc       |Q2|!Q2|Action                     |
     *      |-|--|-|-|---             |---------  |---             |---------  |--|---|---------------------------|
     *      |0|1 |0|0|!(S or Q)=!Q    |!(0 or 0)=1|!(R or !Q)=Q    |!(0 or 1)=0|0 |1  |HOLD                       |
     *      |0|1 |1|0|!(S or Q)=!Q    |!(1 or 0)=0|!(R or !Q)=Q    |!(0 or 0)=1|1 |0  |SET                        |
     *      |1|0 |0|0|!(S or Q)=!Q    |!(0 or 1)=0|!(R or !Q)=Q    |!(0 or 0)=1|1 |0  |HOLD                       |
     *      |1|0 |0|1|**!(R or !Q)=Q**|!(1 or 1)=0|**!(S or Q)=!Q**|!(0 or 0)=1|0 |1  |RESET                      |
     *      |0|1 |0|0|!(S or Q)=!Q    |!(0 or 0)=1|!(R or !Q)=Q    |!(0 or 1)=0|0 |1  |HOLD                       |
     *      |0|1 |1|1|!(S or Q)=!Q    |!(1 or 0)=0|!(R or !Q)=Q    |!(1 or 0)=0|0 |0  |INVALID, CONTRADICTION     |
     *  - simply, the action table becomes:
     *      |S|R|Action       |
     *      |-|-|-------------|
     *      |0|0|HOLD         |
     *      |1|0|SET          |
     *      |0|1|RESET        |
     *      |1|1|CONTRADICTION|
     */
    if (s) {
        *q_not = NOR(s, *q);    // do the S NOR gate first since S went high
        *q = NOR(r, *q_not);
    } else if (r) {
        *q = NOR(r, *q_not);    // do the R NOR gate first since R went high
        *q_not = NOR(s, *q);
    } else {
        // if both are high, order doesnt actually matter, you end up with nonsense anyway
        *q_not = NOR(s, *q);    // will be 0
        *q = NOR(r, *q_not);    // will be 0
    }
}

void sr_flip_flop(bool clk, bool s, bool r, bool* q, bool* q_not) {
    /**
     * Cannonically its an SR latch gated by clock-gated inputs
     *  - each input is clk gated with AND gates
     *  if clk enabled:
     *      sr_latch(s, r, q, !q)
     *  else:
     *      sr_latch(0, 0, q, !q)  // hold
     */
    bool s1, r1;
    s1 = AND(clk, s);
    r1 = AND(clk, r);
    sr_latch(s1, r1, q, q_not);
}

void jk_flip_flop(bool clk, bool j, bool k, bool* q, bool* q_not) {
    /**
     * Cannonically its an SR latch gated by clk, J, K
     *  with 3 input AND gates on the clock feeding into R, S (SR latch upside down).
     *  NO RACE CONDITION (J = K = 1) means toggle
     *  - R = AND(J, clk, !Q)
     *  - S = AND(K, clk, Q)
     *  - simply, the action table becomes:
     *      |S|R|Action       |
     *      |-|-|-------------|
     *      |0|0|HOLD         |
     *      |1|0|SET          |
     *      |0|1|RESET        |
     *      |1|1|TOGGLE       |
     */
    bool r = AND3(j, clk, *q_not);
    bool s = AND3(k, clk, *q);
    sr_flip_flop(clk, r, s, q, q_not);
}

void t_flip_flop(bool clk, bool t, bool* q, bool* q_not) {
    /**
     * Cannonically its a JK latch whose J/K pins are tied together as T.
     *  - just literally use the non-race condition J = K = 1 = toggle on purpose.
     *  - if T and clock, flip
     *  - else, remember
     *  - simply, the action table becomes:
     *      |T|clk|Action       |
     *      |-|---|-------------|
     *      |0|0  |HOLD         |
     *      |1|0  |HOLD         |
     *      |0|1  |HOLD         |
     *      |1|1  |TOGGLE       |
     */
    jk_flip_flop(clk, t, t, q, q_not);
}

bool counter_1bit(bool x, bool* clk) {
    /**
     * Cannonically just use a single t-ff that just goes up when the clock goes up.
     * NOTE: !q is the actual counter current value, not q...
     * TODO: for now, this thing runs in a loop, but should somehow be syncrhonized globally
     * @param bool x - initial value (counting up from)
     * @param bool* clk
     * @returns bool - when counter is done
     */
    printf("        counter_1bit x=%d, clk=%d\n", x, *clk);
    bool q = x, q_not = !q, t=1;  // initial value
    *clk = 0;
    t_flip_flop(*clk, t, &q, &q_not);  // initialized

    size_t i = 0;
    printf("            counter_1bit itr=%02lu: clk=%d, t=%d, q=%d, !q=%d\n", i, *clk, t, q, q_not);
    do {
        *clk ^= 1;  // toggle edge
        t_flip_flop(*clk, t, &q, &q_not);
        i += 1;
        printf("            counter_1bit itr=%02lu: clk=%d, t=%d, q=%d, !q=%d\n", i, *clk, t, q, q_not);
    } while(q != 0);  // while latch has not reset just yet
    return true;
}

bool counter_2bit(bool x1, bool x0, bool* clk) {
    /**
     * Cannonically it's 2 t-ff stitched together, when 1st resets, the next toggles.
     * NOTE: !q is the actual counter current value, not q...
     * TODO: for now, this thing runs in a loop, but should somehow be syncrhonized globally
     * @param bool x1 - initial value (counting up from)
     * @param bool x0 - initial value (counting up from)
     * @param bool* clk
     * @returns bool - when counter is done
     */
    printf("        counter_2bit x10=%d%d, clk=%d\n", x1, x0, *clk);
    bool q0 = x0, q0_not = !q0, t0=1;  // initial values
    bool q1 = x1, q1_not = !q1, t1=q0;
    *clk = 0;
    t_flip_flop(*clk, t0, &q0, &q0_not);  // initialized
    t_flip_flop(*clk, t1, &q1, &q1_not);

    size_t i = 0;
    printf("            counter_2bit itr=%02lu: clk=%d, t10=%d%d, q10=%d%d, !q10=%d%d\n", i, *clk, t1, t0, q1, q0, q1_not, q0_not);
    do {
        *clk ^= 1;  // toggle edge
        t_flip_flop(*clk, t0, &q0, &q0_not);
        t1 = q0;
        t_flip_flop(*clk, t1, &q1, &q1_not);
        i += 1;
        printf("            counter_2bit itr=%02lu: clk=%d, t10=%d%d, q10=%d%d, !q10=%d%d\n", i, *clk, t1, t0, q1, q0, q1_not, q0_not);
    } while(q0 != 0 || q1 != 0);  // while latch has not reset just yet

    return true;
}

bool counter_3bit(bool x2, bool x1, bool x0, bool* clk) {
    /**
     * Cannonically it's 3 t-ff stitched together, when 1st AND 2nd resets, the 3rd toggles.
     * NOTE: !q is the actual counter current value, not q...
     * TODO: for now, this thing runs in a loop, but should somehow be syncrhonized globally
     * @param bool x2 - initial value (counting up from)
     * @param bool x1 - initial value (counting up from)
     * @param bool x0 - initial value (counting up from)
     * @param bool* clk
     * @returns bool - when counter is done
     */
    printf("        counter_3bit x210=%d%d%d, clk=%d\n", x2, x1, x0, *clk);
    bool q0 = x0, q0_not = !q0, t0=1;  // initial values
    bool q1 = x1, q1_not = !q1, t1=q0;
    bool q2 = x2, q2_not = !q2, t2=q0;
    *clk = 0;
    t_flip_flop(*clk, t0, &q0, &q0_not);  // initialized
    t_flip_flop(*clk, t1, &q1, &q1_not);
    t_flip_flop(*clk, t2, &q2, &q2_not);

    size_t i = 0;
    printf("            counter_3bit itr=%02lu: clk=%d, t210=%d%d%d, q210=%d%d%d, !q210=%d%d%d\n", i, *clk, t2, t1, t0, q2, q1, q0, q2_not, q1_not, q0_not);
    do {
        *clk ^= 1;  // toggle edge
        t_flip_flop(*clk, t0, &q0, &q0_not);
        t1 = q0;
        t_flip_flop(*clk, t1, &q1, &q1_not);
        t2 = AND(q0, q1);
        t_flip_flop(*clk, t2, &q2, &q2_not);
        i += 1;
        printf("            counter_3bit itr=%02lu: clk=%d, t210=%d%d%d, q210=%d%d%d, !q210=%d%d%d\n", i, *clk, t2, t1, t0, q2, q1, q0, q2_not, q1_not, q0_not);
    } while(q0 != 0 || q1 != 0 || q2 != 0);  // while latch has not reset just yet

    return true;
}

bool counter_4bit(bool x3, bool x2, bool x1, bool x0, bool* clk) {
    /**
     * Cannonically it's 4 t-ff stitched together
     *  - when 1st AND 2nd resets, the 3rd toggles.
     *  - when 1st AND 2nd AND 3rd resets, the 4th toggles.
     * NOTE: !q is the actual counter current value, not q...
     * USAGE: counts from [2^n - 1 - initial - 1, 2^n - 1]
     *  - if you set the counter to 3
     *      - you'll have 3 iterations
     *      - count up from 1101 to 1111 (3 total numbers), [13, 14, 15]
     *  - if you set counter to 9,
     *      - you'll have 9 iterations
     *      - count up from 0111 to 1111 (9 total numbers), [7, 8, 9, 10, 11, 12, 13, 14, 15]
     * TODO: for now, this thing runs in a loop, but should somehow be syncrhonized globally
     * @param bool x3 - initial value (counting up from)
     * @param bool x2 - initial value (counting up from)
     * @param bool x1 - initial value (counting up from)
     * @param bool x0 - initial value (counting up from)
     * @param bool* clk
     * @returns bool - when counter is done
     */
    printf("        counter_4bit x3210=%d%d%d%d, clk=%d\n", x3, x2, x1, x0, *clk);
    bool q0 = x0, q0_not = !q0, t0=1;  // initial values
    bool q1 = x1, q1_not = !q1, t1=q0;
    bool q2 = x2, q2_not = !q2, t2=q0;
    bool q3 = x3, q3_not = !q3, t3=q0;
    *clk = 0;
    t_flip_flop(*clk, t0, &q0, &q0_not);  // initialized
    t_flip_flop(*clk, t1, &q1, &q1_not);
    t_flip_flop(*clk, t2, &q2, &q2_not);
    t_flip_flop(*clk, t3, &q3, &q3_not);

    size_t i = 0;
    printf("            counter_4bit itr=%02lu: clk=%d, t3210=%d%d%d%d, q3210=%d%d%d%d, !q3210=%d%d%d%d\n", i, *clk, t3, t2, t1, t0, q3, q2, q1, q0, q3_not, q2_not, q1_not, q0_not);
    do {
        *clk ^= 1;  // toggle edge
        t_flip_flop(*clk, t0, &q0, &q0_not);
        t1 = q0;
        t_flip_flop(*clk, t1, &q1, &q1_not);
        t2 = AND(q0, q1);
        t_flip_flop(*clk, t2, &q2, &q2_not);
        // t3 = AND3(q0, q1, q2);  // intuitively this, but you can simplify
        t3 = AND(t2, q2);
        t_flip_flop(*clk, t3, &q3, &q3_not);
        i += 1;
    printf("            counter_4bit itr=%02lu: clk=%d, t3210=%d%d%d%d, q3210=%d%d%d%d, !q3210=%d%d%d%d\n", i, *clk, t3, t2, t1, t0, q3, q2, q1, q0, q3_not, q2_not, q1_not, q0_not);
    } while(q0 != 0 || q1 != 0 || q2 != 0 || q3 != 0);  // while latch has not reset just yet

    return true;
}
