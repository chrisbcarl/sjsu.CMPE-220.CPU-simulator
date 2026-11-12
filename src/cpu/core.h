/*  
 *  Author: Donovan Burk
 *  high level cycle() is repeat entry point after core_setup(), call core_teardown() to be nice :)
 *  each step has a function inside cycle()
 *  struct core holds everything? <-- unsure, might just have globals for each step
 *  Need structs for each steps bus and registers
 *  WB struct holds pointers to the registers is changes
 *  IF and MEM need to call mem_access() after filling the MEM bus
*/

typedef struct IF {
    uint32_t pc;
} IF;

typedef struct F_D_bus {
    uint32_t instr;
} F_D_bus;

typedef struct ID {
    uint32_t* registers;
} ID;

// feel free to request more control sigs, will add in small PR's during branch work.
// Only trim at safe points.

typedef struct EX_ctrl {
    bool bypass; // sends data on bus right through
    uint8_t alu_func;   
} EX_ctrl;

typedef struct MEM_ctrl {
    bool write;
    bool read;
    bool bypass;
} MEM_ctrl;

typedef struct WB_ctrl {
    bool reg_wr;
    uint8_t reg_target;
    bool pc_wr;
    bool branch;
    bool jump;  // can act as branch confirm
} WB_ctrl;

typedef struct D_E_bus {
    EX_ctrl ex_ctrl;
    MEM_ctrl mem_ctrl;
    WB_ctrl wb_ctrl;
    uint32_t dataA; // Operand 1, mem address
    uint32_t dataB; // Operand 2
    uint32_t dataC; // ALU output, mem write data, branch target, reg writeback
} D_E_bus;

typedef struct EX {
    // add as needed?
} EX;

typedef struct E_M_bus {
    MEM_ctrl mem_ctrl;
    WB_ctrl wb_ctrl;
    uint32_t dataA;
    uint32_t dataC;
} E_M_bus;

typedef struct MEM {
    // add as needed?
} MEM;

typedef struct MEM_bus {
    bool rd;
    bool wr;
    uint32_t addr;
    uint32_t wr_data;
    uint32_t rd_data;
} MEM_bus;

typedef struct M_W_bus {
    WB_ctrl wb_ctrl;
    uint32_t dataC;
} M_W_bus;

typedef struct WB {
    uint32_t* pc;
    uint32_t** registers;
} WB;

int core_setup();
int cycle();
int core_teardown();

int fetch();
int decode();
int execute();
int memory();
int writeBack();

int mem_access();