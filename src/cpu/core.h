/*
 *  Author: Donovan Burk
 *  2026-11-22 - isa - Chris Carl - externed some of the defines for instructions.h to use
 *  2026-11-19 - isa - Donovan Burk - structs, basic implement, compile, temps
 *  high level cycle() is repeat entry point after core_setup(), call core_teardown() to be nice :)
 *  each step has a function inside cycle()
 *  struct core holds everything? <-- unsure, might just have globals for each step
 *  Need structs for each steps bus and registers
 *  WB struct holds pointers to the registers is changes
 *  IF and MEM need to call mem_access() after filling the MEM bus
 *  Steps should only ever touch their struct, read from the previous bus, and and write to the next bus
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "isa.h"

#define RAMSIZE 1024

extern uint32_t* RAM;

typedef struct IF {
    uint32_t pc;
} IF;

typedef struct ID {
    uint32_t* registers;
    uint8_t flags;          // LSB to MSB: ZCO
} ID;

typedef struct EX {
    // add as needed?
    uint8_t temp;
} EX;

typedef struct MEM {
    // add as needed?
    uint8_t temp;
} MEM;

typedef struct WB {
    uint32_t* pc;
    uint32_t** registers;
} WB;

// feel free to request more control sigs, will add in small PR's during branch work.
// Only trim at safe points.

typedef struct EX_ctrl {
    bool alu;
    bool bypass; // sends data on bus right through
    uint8_t ops;
    uint8_t ins;
} EX_ctrl;

typedef struct MEM_ctrl {
    bool write;
    bool read;
    bool bypass;
} MEM_ctrl;

typedef struct WB_ctrl {
    bool reg_wr;
    uint8_t reg_target;
    bool pc_inc;
    bool pc_wr;
    bool branch;
    bool jump;  // can act as branch confirm
} WB_ctrl;

typedef struct F_D_bus {
    uint32_t ir;
} F_D_bus;
extern F_D_bus FDbus;

typedef struct D_E_bus {
    EX_ctrl ex_ctrl;
    MEM_ctrl mem_ctrl;
    WB_ctrl wb_ctrl;
    uint32_t dataA; // Operand 1, mem address
    uint32_t dataB; // Operand 2
    uint8_t flags;
} D_E_bus;
extern D_E_bus DEbus;

typedef struct E_M_bus {
    EX_ctrl ex_ctrl;
    MEM_ctrl mem_ctrl;
    WB_ctrl wb_ctrl;
    uint32_t dataA;
    uint32_t dataB;
    uint32_t dataC; // ALU output, mem write data, branch target, reg writeback
    uint8_t flags;
} E_M_bus;
extern E_M_bus EMbus;

typedef struct MEM_bus {
    MEM_ctrl mem_ctrl;
    WB_ctrl wb_ctrl;
    bool rd;
    bool wr;
    uint32_t addr;
    uint32_t wr_data;
    uint32_t rd_data;
    uint8_t flags;
} MEM_bus;
extern MEM_bus MEMbus;

typedef struct M_W_bus {
    MEM_ctrl mem_ctrl;
    WB_ctrl wb_ctrl;
    uint32_t dataC;
    uint8_t flags;
} M_W_bus;
extern M_W_bus MWbus;

int core_setup();
void core_reset();
int core_cycle();
int core_teardown();

// TODO: Move definitions and implementation to own files?
int fetch();
int decode();
int execute();
uint8_t alu_flags_deassert(uint8_t flags);
uint8_t alu_flags_assert(uint32_t rd, uint32_t rs1, uint32_t rs2, uint8_t flags);
int memory();
int writeBack();

int mem_access();
uint32_t get_register_value(uint8_t reg_target);
uint8_t get_flags();
void set_register_value(uint8_t reg_target, uint32_t value);
void set_flags(uint8_t value);

void core_dump(uint32_t cycle, uint32_t total_addresses);
int core_run(uint32_t clocks, bool no_dumps, bool dump_every_clock, bool break_on_halt, uint32_t addresses_around_pc);
