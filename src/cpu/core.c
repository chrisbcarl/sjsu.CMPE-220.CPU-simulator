/*
 *  Author: Donovan Burk
 *  2026-11-25 - Chris Carl - core.c - closer in line with electrical reality rather than using functions, added reset, get/set register values, get/set flags
 *  2026-11-23 - Chris Carl - core.c - fetch "errors" if pc >= RAMSIZE, added get_register_value, get_flags, run
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
#include <stddef.h>

#include "common.h"
#include "core.h"
#include "isa.h"

// not externed, private
IF if_s;
ID id_s;
EX ex_s;
MEM mem_s;
WB wb_s;

// externed, public
uint32_t* RAM;  // L1 "RAM"

F_D_bus FDbus;
D_E_bus DEbus;
E_M_bus EMbus;
M_W_bus MWbus;

MEM_bus MEMbus;

int core_setup() {
    /**
     * @authors Donovan Burk, Chris Carl
     * Allocate memory for the core object.
     * NOTE: must be called first! Please call core_teardown() later?
     *
     * @return int - Whether core allocated or not. Would only fail in extreme OS circumstances.
     */

    id_s.registers = calloc(ISA_NUM_GPRS, sizeof(uint32_t));
    if (id_s.registers == NULL) return EXIT_FAILURE;

    RAM = calloc(RAMSIZE, sizeof(uint32_t));
    if (RAM == NULL) return EXIT_FAILURE;

    core_reset();
    return 0;
}

void core_reset() {
    /**
     * @authors Chris Carl
     * Set all values to 0 except the ones necessary to get the core running
     *      pc_inc needs to be high so pc can increment at all
     *      everything else is 0'd
     */

    // stage reset
    if_s.pc = 0x0;
    id_s.flags = 0;
    for (size_t i = 0; i < ISA_NUM_GPRS; i++) {id_s.registers[i] = 0;}

    ex_s.temp = 0;
    mem_s.temp = 0;
    wb_s.pc = &if_s.pc;
    wb_s.registers = &id_s.registers;

    for (size_t i = 0; i < RAMSIZE; i++) {RAM[i] = 0;}

    // bus reset
    FDbus.ir = 0x0;

    DEbus.ex_ctrl.alu = false;
    DEbus.ex_ctrl.bypass = false;
    DEbus.ex_ctrl.ops = 0;
    DEbus.ex_ctrl.ins = 0;
    DEbus.mem_ctrl.write = false;
    DEbus.mem_ctrl.read = false;
    DEbus.mem_ctrl.bypass = false;
    DEbus.wb_ctrl.reg_wr = false;
    DEbus.wb_ctrl.reg_target = 0;
    DEbus.wb_ctrl.pc_inc = true;  // we want the pc to increment after reset
    DEbus.wb_ctrl.pc_wr = false;
    DEbus.wb_ctrl.branch = false;
    DEbus.wb_ctrl.jump = false;
    DEbus.dataA = 0;
    DEbus.dataB = 0;
    DEbus.flags = 0;

    EMbus.ex_ctrl = DEbus.ex_ctrl;
    EMbus.mem_ctrl = DEbus.mem_ctrl;
    EMbus.wb_ctrl = DEbus.wb_ctrl;
    EMbus.dataA = 0;
    EMbus.dataB = 0;
    EMbus.dataC = 0;
    EMbus.flags = 0;

    MEMbus.mem_ctrl = EMbus.mem_ctrl;
    MEMbus.wb_ctrl = EMbus.wb_ctrl;
    MEMbus.rd = false;
    MEMbus.wr = false;
    MEMbus.addr = 0x0;
    MEMbus.wr_data = 0;
    MEMbus.rd_data = 0;
    MEMbus.flags = 0;

    MWbus.mem_ctrl = MEMbus.mem_ctrl;
    MWbus.wb_ctrl = MEMbus.wb_ctrl;
    MWbus.dataC = 0;
    MWbus.flags = 0;
}

int core_cycle() {
    /**
     * @authors Donovan Burk, Chris Carl
     * Canonical fetch-decode-execute cycle.
     *      Should a stage fail, the return value is that stage's index. ex) return 3 -> execute() failed
     *
     * @return int - Whether a core_cycle successfully executed all stages. If not, the stage number from 1-5
     */

    if(fetch()) return 1;
    if(decode()) return 2;
    if(execute()) return 3;
    if(memory()) return 4;
    if(writeBack()) return 5;
    return 0;
}

int core_teardown() {
    /**
     * @authors Donovan Burk, Chris Carl
     * De-allocate the core object.
     *
     * @return int - Whether teardown successful or not
     */

    free(id_s.registers);
    free(RAM);
    return 0;
}

int fetch() {
    /**
     * @authors Donovan Burk, Chris Carl
     * "Fetch memory at PC, load to IR, increment PC"
     *
     * @return int - Whether this stage was successful or not
     */

    FDbus.ir = RAM[if_s.pc];
    mem_access();
    // BUG: once turned off, you can't un-halt...
    if (MWbus.wb_ctrl.pc_inc) {
        if_s.pc += 1;  // 4 byte increment since +(1 x 32 bits)
        if (if_s.pc == RAMSIZE) {
            // if_s.pc = 0;  // or just loopback
            fprintf(stderr, "ERROR: pc incremented above RAMSIZE! pc = 0x%x\n", if_s.pc);
            return 1;
        }
    }
    return 0;
}

int decode() {
    /**
     * @authors Donovan Burk, Chris Carl
     * Decode the IR's opcode, set control signals
     *      If an instruction can set dataA or dataB already, do so
     *      If an instruction needs shifted data, do so.
     *      Set bypasses on the stages that wont contribute to the instruction's goals
     *
     * @return int - Whether this stage was successful or not
     */

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

    // data from previous stage (none, gotta init)
    DEbus.wb_ctrl.reg_wr = false;
    DEbus.wb_ctrl.reg_target = reg_target;
    DEbus.wb_ctrl.branch = jmp && (ops == 3) && alu;
    DEbus.wb_ctrl.jump = jmp;
    DEbus.wb_ctrl.pc_inc = true;  // only turned off for HALT
    DEbus.wb_ctrl.pc_wr = DEbus.wb_ctrl.jump | DEbus.wb_ctrl.branch;

    DEbus.ex_ctrl.alu = alu;
    DEbus.ex_ctrl.bypass = false;
    DEbus.ex_ctrl.ops = ops;
    DEbus.ex_ctrl.ins = ins;

    if (jmp == 0 && ops == 0 && alu == 0 && ins == 0) {
        // HALT
        DEbus.ex_ctrl.bypass = true;
        DEbus.mem_ctrl.bypass = true;
        DEbus.wb_ctrl.reg_wr = false;
        DEbus.wb_ctrl.pc_inc = false;

    } else if (DEbus.ex_ctrl.alu) {
        DEbus.wb_ctrl.reg_wr = true;

        DEbus.dataA = id_s.registers[reg_rs1];
        DEbus.dataB = id_s.registers[reg_rs2];
        DEbus.flags = id_s.flags;
    } else {
        if (ops == 2) {
            // LOAD, STORE, MOV, LI, LUI
            switch (DEbus.ex_ctrl.ins) {
                case (INS_LOAD): {
                    break;
                }
                case (INS_STORE): {
                    break;
                }
                case (INS_MOV): {
                    break;
                }
                default: {
                    // LI, LUI
                    DEbus.dataA = 0;
                    DEbus.dataB = (INS_LI == DEbus.ex_ctrl.ins) ? imm : imm << 16;
                    DEbus.ex_ctrl.bypass = false;   // i need the ALU
                    DEbus.ex_ctrl.ins = INS_ADD;  // i need to add 0 + imm
                    DEbus.mem_ctrl.bypass = true;  // dont need mem
                    DEbus.wb_ctrl.reg_wr = true;  // need to write back
                    DEbus.wb_ctrl.reg_target = reg_target;  // to this register
                    break;
                }
            }
        }
    }

    return 0;
}

int execute() {
    /**
     * @authors Donovan Burk, Chris Carl
     * ALU needed for this? Do the specific ALU instruction
     *      Bypassable!
     *
     * @return int - Whether this stage was successful or not
     */

    // data from previous stage
    EMbus.ex_ctrl = DEbus.ex_ctrl;
    EMbus.mem_ctrl = DEbus.mem_ctrl;
    EMbus.wb_ctrl = DEbus.wb_ctrl;
    EMbus.dataA = DEbus.dataA;
    EMbus.dataB = DEbus.dataB;
    EMbus.dataC = 0;
    EMbus.flags = DEbus.flags;

    if (EMbus.ex_ctrl.bypass) return 0;  // do nothing!

    // ONLY ALU operations in this switch!!!
    EMbus.flags = alu_flags_deassert(EMbus.flags);  // always deassert the flags
    switch (EMbus.ex_ctrl.ins) {
        /* ----- arithmetic: 3 operands ----- */
        case (INS_ADD): EMbus.dataC = EMbus.dataA + EMbus.dataB; break;
        default: break;  // shouldn't happen
    }
    EMbus.flags = alu_flags_assert(EMbus.dataC, EMbus.dataA, EMbus.dataB, EMbus.flags);  // always check if we should assert some flags

    return 0;
}

uint8_t alu_flags_deassert(uint8_t flags) {
    /**
     * @authors Chris Carl
     * Set arithmetic flags are low
     *
     * @param flags uint8_t - any 8-bit number
     * @return uint8_t - new flags value
     */

    flags &= !FLAG_Z;
    flags &= !FLAG_C;
    flags &= !FLAG_O;
    return flags;
}

uint8_t alu_flags_assert(uint32_t rd, uint32_t rs1, uint32_t rs2, uint8_t flags) {
    /**
     * @authors Chris Carl
     * If rd's value was due to what rs1 and rs2 were, Zero, Carry, or Overflow might have happend.
     *      Detect and mark it so.
     * NOTE: not the real rd, rs1, rs2 or flags, could be any numbers (and pointers wouldnt be faster, these are small things anyway)
     *
     * @param rd uint32_t - any 32-bit number
     * @param rs1 uint32_t - any 32-bit number
     * @param rs2 uint32_t - any 32-bit number
     * @param flags uint8_t - any 8-bit number
     * @return uint8_t - new flags value
     */

    // ZERO - high if rd is zero.
    if (rd == 0) flags |= FLAG_Z;

    // CARRY - unsigned, high if went above 0xFFFF, direction upwards
    // in unsigned, if rs2 is bigger than rs1 enough to go over 0xF...F, then carry happened
    // printf("rs2 0x%x, 0xFFFFFFFF - rs1 0x%x, condition %d\n", rs2, 0xFFFFFFFF - rs1, rs2 > (0xFFFFFFFF - rs1));
    if (rs2 > (0xFFFFFFFF - rs1)) flags |= FLAG_C;  // in hardware this is handled instantly due to full adder's C output

    // OVERFLOW - signed, two negatives should be negative, two positives should be positive, otherwise overflow
    // in signed, two negs equals neg, two pos equals pos, opposite can't overflow
    // 1 + 1 = 1, 0 + 0 = 0, 0 + 1 = X, anything else is bad....
    bool rs1_sign = (rs1 >> 31) & 1;
    bool rs2_sign = (rs2 >> 31) & 1;
    bool rd_sign = (rd >> 31) & 1;
    // printf("rs1_sign %d | rs2_sign %d | rd_sign %d\n", rs1_sign, rs2_sign, rd_sign);
    if (rs1_sign == rs2_sign) {
        if (rd_sign != rs1_sign)  flags |= FLAG_O;
    }
    return flags;
}

int memory() {
    /**
     * @authors Donovan Burk, Chris Carl
     * Alter the memory with data
     *      Bypassable!
     *
     * @return int - Whether this stage was successful or not
     */

    // data from previous stage
    MEMbus.mem_ctrl = EMbus.mem_ctrl;
    MEMbus.wb_ctrl = EMbus.wb_ctrl;
    MEMbus.rd = EMbus.mem_ctrl.read;
    MEMbus.wr = EMbus.mem_ctrl.write;
    MEMbus.addr = EMbus.dataA;
    // MEMbus.wr_data;
    // MEMbus.rd_data;
    MEMbus.flags = EMbus.flags;

    if (MEMbus.mem_ctrl.bypass) return 0;  // do nothing!

    mem_access();
    return 0;
}

int writeBack() {
    /**
     * @authors Donovan Burk, Chris Carl
     * If the computed data is supposed to go to a register, write it back
     *      Is the PC supposed to be altered?
     *      Bypassable!
     *
     * @return int - Whether this stage was successful or not
     */

    MWbus.mem_ctrl = EMbus.mem_ctrl;
    MWbus.wb_ctrl = EMbus.wb_ctrl;
    MWbus.dataC = EMbus.dataC;
    MWbus.flags = EMbus.flags;


    id_s.flags = MWbus.flags;
    if (MWbus.wb_ctrl.reg_wr) {
        (*wb_s.registers)[MWbus.wb_ctrl.reg_target] = MWbus.dataC;
    } else if (MWbus.wb_ctrl.pc_wr) {
        // TODO: fill this out for branch and jump, sign extend stuff
        // if_s.pc = MWbus.dataC;
    }
    // TODO: if (/ branch */) *(wb_s.pc) = MWbus.dataC;
    return 0;
}

int mem_access() {
    // TODO: is this supposed to be a locking mechanism or fence?
    return 0;
}

void core_dump(uint32_t clock, uint32_t total_addresses) {
    /**
     * @authors Chris Carl
     * Basic dump of core register and memory contents.
     * NOTE: i'm sure I did the sliding window poorly, but hey, math ain't my strong suit
     *
     * @param clock uint32_t - the clock number, entirely arbitrary
     * @param total_addresses uint32_t - how much around the PC would you like to see? 5 address around the PC makes sense
     */

    // mnemonic operands dump
    printf("    CLK %u\n", clock);
    isa_disassemble_2(FDbus.ir, COMMON_BUFFER_0, COMMON_BUFFER_LEN);
    printf("    \"%s\"\n", COMMON_BUFFER_0);

    // register dump
    printf("    GPRS: ");
    for (size_t i = 0; i < ISA_NUM_GPRS / 2; i++) {
        printf("R%lu: 0x%08x | ", i, id_s.registers[i]);
    }
    printf("\n          ");
    for (size_t i = ISA_NUM_GPRS / 2; i < ISA_NUM_GPRS; i++) {
        printf("R%lu: 0x%08x | ", i, id_s.registers[i]);
    }
    printf("\n    SPRS: PC: 0x%08x | IR: 0x%08x | FLAGS: 0x%08x - Z: %01d | C: %01d | O: %01d", if_s.pc, FDbus.ir, id_s.flags, FLAG_Z_OF(id_s.flags), FLAG_C_OF(id_s.flags), FLAG_O_OF(id_s.flags));

    // memory dump (sliding window of total_addresses around PC)
    uint32_t pc = if_s.pc - 1;  // by now, the clock has already advanced the PC += 1 already, so backup once to get the original memory location
    printf("\n    MEM (%u ~[PC-1]@0x%x):\n", total_addresses, pc);
    uint32_t start = 0, end = 0;
    if (!(pc < total_addresses) && !(pc > (RAMSIZE - (total_addresses - 1)))) {  // most common
        // say total is 5, PC = 10 | dumping 0x7 - 0xB
        start = pc - total_addresses / 2;
        end = pc + total_addresses / 2;
        // printf("if:      start %u, end %u\n", start, end);
    } else if (pc < total_addresses) {  // early game
        // say total is 5, PC = -3 | dumping 0xFFFF FFFA - 0xFFFF FFFF
        start = pc - total_addresses / 2;
        if (start > total_addresses) start = 0;  // overflow
        end = start + (total_addresses - 1);
        // printf("else if: start %u, end %u\n", start, end);
    } else {  // endgame (pc > (RAMSIZE - (total_addresses - 1)))
        // say total is 5, PC = 3 | dumping 0x0 - 0x5
        start = RAMSIZE - total_addresses;
        end = RAMSIZE - 1;
        // printf("else:    start %u, end %u\n", start, end);
    }
    printf("        addr        contents  \n");
    while (end != start) {
        // descending address order
        printf("        0x%08x  0x%08x\n", end, RAM[end]);
        end -= 1;
    }
    printf("        0x%08x  0x%08x\n", start, RAM[start]);
}

const char* STAGES[] = {
    [1] = "fetch",
    [2] = "decode",
    [3] = "execute",
    [4] = "memory",
    [5] = "writeBack",
};

int core_run(uint32_t clocks, bool dump_final_clock, bool dump_every_clock, bool break_on_halt, uint32_t addresses_around_pc) {
    /**
     * @authors Chris Carl
     * Actually run this simulated core that can execute our ISA if stuff is in RAM!
     * NOTE: must have run core_setup() first! please run core_teardown() after?
     *
     * @param clocks uint32_t - how many clocks do you want to run for?
     * @param dump_final_clock bool - want the last dump?
     * @param dump_every_clock bool - want a core dump printed every clock?
     * @param break_on_halt bool - if you see HALT instruction, should we return from core_run?
     * @param addresses_around_pc uint32_t - in the RAM portion of core_dump, how many addresses around PC to print?
     * @return int - EXIT_SUCCESS or the stage it failed on
     */

    uint32_t clk = 0;
    int res = 0;
    while (res == 0 && clk < clocks) {
        int res = core_cycle();
        if (dump_every_clock) core_dump(clk, addresses_around_pc);
        if (res != 0) {
            fprintf(stderr, "ERROR: failed at stage '%s', clock %u!\n", STAGES[res], clk);
            return res;
        }
        clk += 1;
        if (RAM[if_s.pc] == 0 && break_on_halt) break;
    };
    if (dump_final_clock && !dump_every_clock) core_dump(clk, addresses_around_pc);
    return EXIT_SUCCESS;
}

uint32_t get_register_value(uint8_t reg_target) {
    /**
     * @authors Chris Carl
     * Core registers are "private", so this func allows you to get values.
     *
     * @param reg_target uint8_t - registers [0, 15]
     * @return uint32_t - R[reg_target]'s value
     */

    return id_s.registers[reg_target];
}

uint8_t get_flags() {
    /**
     * @authors Chris Carl
     * Core flags are "private", so this func allows you to get values.
     *
     * @return uint8_t - flag's value
     */

    return id_s.flags;
}

void set_register_value(uint8_t reg_target, uint32_t value) {
    /**
     * @authors Chris Carl
     * Core registers are "private", so this func allows you to set values.
     *
     * @param reg_target uint8_t - registers [0, 15]
     * @param value uint32_t - value
     */

    id_s.registers[reg_target] = value;
}

void set_flags(uint8_t value) {
    /**
     * @authors Chris Carl
     * Core flags are "private", so this func allows you to set values.
     *
     * @param value uint8_t - value
     */

    id_s.flags = value;
}
