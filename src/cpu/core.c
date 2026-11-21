/*  
 *  Author: Donovan Burk
 *  high level cycle() is repeat entry point after core_setup(), call core_teardown() to be nice :)
 *  each step has a function inside cycle()
 *  struct core holds everything? <-- unsure, might just have globals for each step
 *  Need structs for each steps bus and registers
 *  WB struct holds pointers to the registers is changes
 *  IF and MEM need to call mem_access() after filling the MEM bus
*/

#include "core.h"

IF if_s;
ID id_s;
EX ex_s;
MEM mem_s;
uint32_t* ram;
WB wb_s;

F_D_bus FDbus;
D_E_bus DEbus;
E_M_bus EMbus;
M_W_bus MWbus;

MEM_bus MEMbus;

int core_setup() {
    wb_s.pc = &if_s.pc;
    id_s.registers = calloc(16, 16 * sizeof(uint32_t));
    wb_s.registers = &id_s.registers;
    ram = calloc(RAMSIZE, RAMSIZE * sizeof(uint32_t));
    return 0;
}

int cycle() {
    if(fetch()) return 1;
    if(decode()) return 2;
    if(execute()) return 3;
    if(memory()) return 4;
    if(writeBack()) return 5;
    return 0;
}

int core_teardown() {
    free(id_s.registers);
    free(ram);
    return 0;
}



int fetch() {
    return 0;
}

int decode() {
    return 0;
}

int execute() {
    return 0;
}

int memory() {
    return 0;
}

int writeBack() {
    return 0;
}

int mem_access() {
    return 0;
}
