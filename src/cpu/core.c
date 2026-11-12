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

IF if;
ID id;
EX ex;
MEM mem;
unint32_t* ram;
WB wb;

F_D_bus FDbus;
D_E_bus DEbus;
E_M_bus EMbus;
M_W_bus MWbus;

MEM_bus MEMbus;

int core_setup() {
    wb.pc = &if.pc;
    id.registers = calloc(16 * sizeof(unint32_t));
    wb.registers = &id.registers;
    ram = calloc(RAMSIZE * sizeof(uint32_t));
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
    free(id.registers);
    free(ram);
}