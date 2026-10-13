#ifndef H_GATES
#define H_GATES

// stdlib
#include <stdbool.h>

bool AND(bool x, bool y);
bool OR(bool x, bool y);
bool NOT(bool x);
bool NAND(bool x, bool y);
bool NOR(bool x, bool y);
bool XOR(bool x, bool y);
bool XNOR(bool x, bool y);
void ASSIGN(bool* x, bool* y);
bool AND3(bool x, bool y, bool z);
bool OR3(bool x, bool y, bool z);
bool NAND3(bool x, bool y, bool z);
bool NOR3(bool x, bool y, bool z);
bool XOR3(bool x, bool y, bool z);
bool XNOR3(bool x, bool y, bool z);

#endif
