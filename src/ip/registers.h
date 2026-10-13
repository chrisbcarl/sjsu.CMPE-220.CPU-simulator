#ifndef H_REGISTERS
#define H_REGISTERS

#include <stdbool.h>
#include <stdint.h>

typedef struct flags_t {
    bool carry;
    bool overflow;
} flags_t;

extern flags_t FLAGS;

#define SET(flag) FLAGS.flag = 1
#define RESET(flag) FLAGS.flag = 0
#define TEST(flag) FLAGS.flag == 1

#endif
