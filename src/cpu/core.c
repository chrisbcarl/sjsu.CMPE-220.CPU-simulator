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

int core_setup() {

}

int cycle() {

}

int core_teardown() {

}