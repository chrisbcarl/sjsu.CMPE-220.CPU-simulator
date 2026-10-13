#ifndef H_LOGIC
#define H_LOGIC

// stdlib
#include <stdbool.h>

void sr_latch(bool s, bool r, bool* q, bool* q_not);
void sr_flip_flop(bool clk, bool s, bool r, bool* q, bool* q_not);
void jk_flip_flop(bool clk, bool j, bool k, bool* q, bool* q_not);
void t_flip_flop(bool clk, bool t, bool* q, bool* q_not);
bool counter_1bit(bool x, bool* clk);
bool counter_2bit(bool x1, bool y0, bool* clk);
bool counter_3bit(bool x2, bool x1, bool x0, bool* clk);
bool counter_4bit(bool x3, bool x2, bool x1, bool x0, bool* clk);

#endif
