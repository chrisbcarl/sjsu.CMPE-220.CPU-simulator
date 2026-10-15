/*
On the principle of simplicity.

Assumptions:
- word size: 32 bit
    - register size: 32bit
    - registers: 32
    - memory size: 2^32

2026-10-14 18:49 - cpu-simple - chriscarl - done
2026-10-14 18:04 - cpu-simple - chriscarl - started on cpu-simple.h, it probably doesnt work out of the box, but hey, looks interesting
*/


#ifndef H_CPU_SIMPLE
#define H_CPU_SIMPLE

#include <stdint.h>  // uint
#include <stddef.h>  // size_t
#include <stdbool.h>

typedef struct memory {
    int32_t* array;
    size_t size;
    size_t ptr;  // most recent memory location (either from read OR write)
} memory;

typedef struct memory_controller {
    memory gpr;
    memory spr;
    memory ram;
} memory_controller;

void memory_controller_read(memory* mem, int32_t from, int32_t to);  // read from an address into a register
void memory_controller_write(memory* mem, int32_t from, int32_t to);  // read from a register into an address

typedef struct bus_bi_directional_bus {
    bool enabled;
    bool read_write;
    memory_controller* mc;

    int32_t data;
    int32_t to;
    int32_t from;
} bus_bi_directional_bus;

void bus_bi_directional_bus_action(bus_bi_directional_bus* bus);  // perform all actions on the bus based on bits?

typedef struct bus_uni_directional {
    bool enabled;
    memory_controller* mc;

    int32_t data;
    int32_t from;
    int32_t to;
} bus_uni_directional;

void bus_uni_directional_action(bus_uni_directional* bus);  // perform all actions on the bus based on bits?

typedef struct bus_two_directional_buses {
    bool enabled;
    memory_controller* mc;

    bus_uni_directional to_memory;
    bus_uni_directional from_memory;
} bus_two_directional_buses;

void bus_two_directional_buses_action(bus_two_directional_buses* bus);  // perform all actions on the bus based on bits?

typedef struct bus_three_way {
    bool enabled;
    memory_controller* mc;

    bus_bi_directional_bus bi;
    bus_two_directional_buses duo;
} bus_three_way;

void bus_three_way_action(bus_three_way* bus);  // perform all actions on the bus based on bits?


typedef struct ALU {
    memory_controller* mc;

    int from;
    int to;
    int x;
    int y;
    int operation;
} ALU;

void alu_action(ALU* alu);  // act based on the operation bits

typedef struct CPU {
    memory_controller mc;
    bus_three_way bus;
    ALU alu;
    size_t clk;  // the rising edge is mod 0, else mod 1
} CPU;

CPU cpu_create(size_t word_size, size_t gprs, size_t sprs, size_t ram_bytes);
void clock_advance(CPU* cpu);

#endif
