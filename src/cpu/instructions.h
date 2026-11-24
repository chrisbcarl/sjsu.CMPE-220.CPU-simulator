#ifndef H_INSTRUCTIONS
#define H_INSTRUCTIONS

#include <stdint.h>

void LI(uint32_t* rd, uint16_t* imm);
void LUI(uint32_t* rd, uint16_t* imm);
void ADD(uint32_t* rd, uint32_t* rs1, uint32_t* rs2, uint8_t* flags);

#endif
