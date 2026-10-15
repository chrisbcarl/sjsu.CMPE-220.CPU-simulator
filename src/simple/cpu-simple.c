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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cpu-simple.h>

#define WORD_SIZE = 32

CPU cpu_create(size_t word_size, size_t gprs, size_t sprs, size_t ram_bytes) {
    CPU cpu = {
        // memory_controller
        .mc={
            .ram={.array = calloc(ram_bytes, sizeof(int32_t)),  .size=ram_bytes, .ptr = 0},
            .gpr={.array = calloc(gprs, sizeof(int32_t)),       .size=ram_bytes, .ptr = 0},
            .spr={.array = calloc(sprs, sizeof(int32_t)),       .size=ram_bytes, .ptr = 0},
        },
        // bus_three_way
        .bus={
            .enabled=false,
            .mc=NULL,
            .bi={.enabled=false, .read_write=false, .data=0, .from=0, .to=0, .mc=NULL},
            .duo={
                .enabled=false,
                .mc=NULL,
                .from_memory={.enabled=false, .mc=NULL, .from=0, .to=0,},
                .to_memory={.enabled=false, .mc=NULL, .from=0, .to=0,},
            }
        },
        //
        .alu={.operation=0, .mc=NULL, .from=0, .to=0, .x=0, .y=0},
        .clk=false,
    };
    memory_controller* mc = &cpu.mc;
    cpu.bus.bi.mc=mc; cpu.bus.duo.from_memory.mc=mc; cpu.bus.duo.to_memory.mc=mc;
    cpu.alu.mc=mc;
}
