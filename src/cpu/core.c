/*
 *  Author: Donovan Burk
 *  2026-11-22 - Chris Carl - core.c - dummy implementation for fetch/decode/execute/mem/wb (not so dummy)
 *  2026-11-19 - Donovan Burk - core.c - structs, basic implement, compile, temps
 *  high level cycle() is repeat entry point after core_setup(), call core_teardown() to be nice :)
 *  each step has a function inside cycle()
 *  struct core holds everything? <-- unsure, might just have globals for each step
 *  Need structs for each steps bus and registers
 *  WB struct holds pointers to the registers is changes
 *  IF and MEM need to call mem_access() after filling the MEM bus
*/
#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "core.h"
#include "isa.h"
#include "instructions.h"  // circular but supported in C thanks to ifndef

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
    id_s.registers = calloc(ISA_NUM_GPRS, sizeof(uint32_t));
    for (size_t i = 0; i < ISA_NUM_GPRS; i++) {id_s.registers[i] = 0;}

    wb_s.registers = &id_s.registers;
    ram = calloc(RAMSIZE, sizeof(uint32_t));
    for (size_t i = 0; i < RAMSIZE; i++) {ram[i] = 0;}
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
    FDbus.ir = ram[if_s.pc];
    if_s.pc += 1;  // 4 byte increment since +(1 x 32 bits)
    return 0;
}

int decode() {
    uint8_t opcode = (FDbus.ir >> ISA_OPCODE_SHIFT) & ISA_OPCODE_MASK;
    uint8_t jmp = ISA_OP_JMP(opcode);
    uint8_t ops = ISA_OP_OPS(opcode);
    uint8_t alu = ISA_OP_ALU(opcode);
    uint8_t ins = ISA_OP_INS(opcode);
    uint8_t reg_target = (FDbus.ir >> ISA_RD_SHIFT) & ISA_REG_MASK;
    uint8_t reg_rs1 = (FDbus.ir >> ISA_RS1_SHIFT) & ISA_REG_MASK;
    uint8_t reg_rs2 = (FDbus.ir >> ISA_RS2_SHIFT) & ISA_REG_MASK;
    uint16_t imm = (FDbus.ir & ISA_IMM_MASK);
    // uint32_t addr = FDbus.ir & ISA_ADDR_MASK;

    DEbus.wb_ctrl.jump = jmp;
    DEbus.wb_ctrl.branch = jmp & (opcode >= 0xF0);
    DEbus.ex_ctrl.operand_count = ops;
    DEbus.ex_ctrl.instruction_number = ins;

    if (alu) {
        DEbus.dataA = id_s.registers[reg_rs1];
        DEbus.dataB = id_s.registers[reg_rs2];
        DEbus.ex_ctrl.alu_func = ins;
        DEbus.ex_ctrl.bypass = true;
        DEbus.wb_ctrl.reg_wr = true;
        DEbus.wb_ctrl.reg_target = reg_target;
        DEbus.flags = id_s.flags;
    } else {
        if (ops == 2) {
            // LOAD, STORE, MOV, LI, LUI
            if (ins >= 3) {  // LI, LUI
                DEbus.dataA = 0;
                DEbus.dataB = imm;
                DEbus.mem_ctrl.bypass = true;
                DEbus.ex_ctrl.bypass = true;
                DEbus.wb_ctrl.reg_wr = true;
                DEbus.wb_ctrl.reg_target = reg_target;
            }
        }
    }

    return 0;
}

int execute() {
    EMbus.ex_ctrl = DEbus.ex_ctrl;  // data from previous stage
    EMbus.mem_ctrl = DEbus.mem_ctrl;
    EMbus.wb_ctrl = DEbus.wb_ctrl;
    EMbus.dataA = DEbus.dataA;
    EMbus.dataB = DEbus.dataB;
    EMbus.flags = DEbus.flags;

    void (*itype)(uint32_t* rd, uint16_t* imm);
    void (*binary)(uint32_t* rd, uint32_t* rs1, uint8_t* flags);
    void (*trinary)(uint32_t* rd, uint32_t* rs1, uint32_t* rs2, uint8_t* flags);

    if (EMbus.ex_ctrl.operand_count == 0) {
        if (EMbus.ex_ctrl.instruction_number == 0 && EMbus.ex_ctrl.alu_func == 0) {
            // NOP
            MEMbus.rd_data = false;
            MEMbus.wr_data = false;
            MWbus.mem_ctrl.bypass = true;
        }
    } else if (EMbus.ex_ctrl.alu_func) {
        if (EMbus.ex_ctrl.operand_count == 2) {
            binary = INSTRUCTIONS_ARITHMETIC_2_OPS[EMbus.ex_ctrl.instruction_number];
            binary(&EMbus.dataC, &EMbus.dataA, &EMbus.flags);
        } else {
            trinary = INSTRUCTIONS_ARITHMETIC_3_OPS[EMbus.ex_ctrl.instruction_number];
            trinary(&EMbus.dataC, &EMbus.dataA, &EMbus.dataB, &EMbus.flags);
        }
    } else {
        if (EMbus.ex_ctrl.operand_count == 2) {
            if (EMbus.dataA == 0) {
                // LI, LUI
                itype = INSTRUCTIONS_MEMORY[EMbus.ex_ctrl.instruction_number];
                itype(&EMbus.dataC, (uint16_t*)&EMbus.dataB);
            }
        }
    }

    // NOTE: since this is a simulator, NEARLY EVERYTHING gets done in the execute...
    if (EMbus.ex_ctrl.bypass) return 0;  // do nothing!

    return 0;
}

int memory() {
    MEMbus.rd = EMbus.mem_ctrl.read;
    MEMbus.wr = EMbus.mem_ctrl.write;
    MEMbus.addr = EMbus.dataA;
    // MEMbus.wr_data;
    // MEMbus.rd_data;
    MEMbus.flags = EMbus.flags;

    if (MWbus.mem_ctrl.bypass) return 0; // bypassed!

    mem_access();
    return 0;
}

int writeBack() {
    MWbus.mem_ctrl = EMbus.mem_ctrl;
    MWbus.wb_ctrl = EMbus.wb_ctrl;
    MWbus.dataC = EMbus.dataC;
    MWbus.flags = EMbus.flags;

    id_s.flags = MWbus.flags;
    if (MWbus.wb_ctrl.reg_wr) {
        id_s.registers[MWbus.wb_ctrl.reg_target] = MWbus.dataC;
    } else if (MWbus.wb_ctrl.pc_wr) {
        // TODO: fill this out for branch and jump, sign extend stuff
        // if_s.pc = MWbus.dataC;
    }
    return 0;
}

int mem_access() {
    return 0;
}

const void* INSTRUCTIONS_MEMORY[OPCODES] = {
    [3] = LI,
    [4] = LUI,
};
const void* INSTRUCTIONS_ARITHMETIC_2_OPS[OPCODES] = {
    // [0] = NOT,
};
const void* INSTRUCTIONS_ARITHMETIC_3_OPS[OPCODES] = {
    [1] = ADD,
};

void core_dump(uint32_t cycle) {
    printf("core_dump %u\n", cycle);
    isa_disassemble_2(FDbus.ir, BUFFER0, BUFLEN);
    printf("    \"%s\"\n", BUFFER0);
    printf("    GPRS: ");
    for (size_t i = 0; i < ISA_NUM_GPRS / 2; i++) {
        printf("R%lu: 0x%08x | ", i, id_s.registers[i]);
    }
    printf("\n          ");
    for (size_t i = ISA_NUM_GPRS / 2; i < ISA_NUM_GPRS; i++) {
        printf("R%lu: 0x%08x | ", i, id_s.registers[i]);
    }
    printf("\n    SPRS: PC: 0x%08x | IR: 0x%08x | FLAGS: 0x%08x", if_s.pc, FDbus.ir, id_s.flags);
    printf("\n    MEM (+/- 4):\n");
    uint32_t start = 0, end = 0;
    if (if_s.pc - 4 > 0xFFFFFFFB) start = 0; else start = if_s.pc - 4;
    if (if_s.pc + 4 < 3) end = 0xFFFFFFFF; else end = if_s.pc + 4;
    // printf("start %lu, end %lu\n", start, end);
    printf("        addr        contents  \n");
    for(; start < end; start++) {
        printf("        0x%08x  0x%08x\n", start, ram[start]);
    }
}
void clear_busses() {
    DEbus.ex_ctrl.bypass = false;
    DEbus.ex_ctrl.operand_count = 0;
    DEbus.ex_ctrl.alu_func = 0;
    DEbus.ex_ctrl.instruction_number = 0;
    DEbus.mem_ctrl.write = false;
    DEbus.mem_ctrl.read = false;
    DEbus.mem_ctrl.bypass = false;
    DEbus.wb_ctrl.reg_wr = false;
    DEbus.wb_ctrl.reg_target = 0;
    DEbus.wb_ctrl.pc_wr = false;
    DEbus.wb_ctrl.branch = false;
    DEbus.wb_ctrl.jump = false;
    DEbus.dataA = 0;
    DEbus.dataB = 0;
    // DEbus.flags = 0;  // leave as is
    MEMbus.rd = false;
    MEMbus.wr = false;
    MEMbus.addr = 0;
    MEMbus.wr_data = 0;
    MEMbus.rd_data = 0;
    MWbus.mem_ctrl = EMbus.mem_ctrl = DEbus.mem_ctrl;
    EMbus.ex_ctrl = DEbus.ex_ctrl;
    MWbus.wb_ctrl = EMbus.wb_ctrl = DEbus.wb_ctrl;
    EMbus.dataC = 0;
    MWbus.dataC = 0;
}