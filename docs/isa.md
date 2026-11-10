# ISA Table

Author: Jehu Zepeda-Silva <jehu.zepeda-silva@sjsu.edu>

| Date | Author | Change |
|---|---|---|
| 2026-11-09 | Jehu Zepeda-Silva | Instruction table and R/I/J bit layout, matching `isa.h` |

32-bit instructions, 16 registers (`R0`–`R15`), destination leftmost. Source of truth: [`src/cpu/isa.h`](../src/cpu/isa.h).

## Instruction formats

| Type | 31–24 | 23–20 | 19–16 | 15–12 | 11–0 |
|---|---|---|---|---|---|
| **R** | opcode | rd | rs1 | rs2 | unused |
| **I** | opcode | rd | rs1 | imm16 | imm16 |
| **J** | opcode | address24 | address24 | address24 | address24 |

Opcode bits:
```
bit:   7     6  5     4     3  2  1  0
     +-----+------+-----+------------+
     | jmp | ops  | alu |    ins     |
     +-----+------+-----+------------+
```

## Instructions

| Instr | Syntax | Type | jmp | ops | alu | ins | Opcode | Operation | Flags |
|---|---|---|---|---|---|---|---|---|---|
| `HALT` | `HALT` | — | 0 | 0 | 0 | 0 | `0x00` | stop the clock | — |
| `NOP` | `NOP` | — | 0 | 0 | 0 | 1 | `0x01` | do nothing | — |
| `RET` | `RET` | — | 1 | 0 | 0 | 0 | `0x80` | PC ← LR | — |
| `PUSH` | `PUSH rd` | R | 0 | 1 | 0 | 0 | `0x20` | SP ← SP − 1; mem[SP] ← rd | — |
| `POP` | `POP rd` | R | 0 | 1 | 0 | 1 | `0x21` | rd ← mem[SP]; SP ← SP + 1 | — |
| `JUMP` | `JUMP label` | J | 1 | 1 | 0 | 0 | `0xA0` | PC ← address | — |
| `JR` | `JR rd` | R | 1 | 1 | 0 | 1 | `0xA1` | PC ← rd | — |
| `CALL` | `CALL label` | J | 1 | 1 | 0 | 2 | `0xA2` | LR ← PC; PC ← address | — |
| `LOAD` | `LOAD rd, imm(rs1)` | I | 0 | 2 | 0 | 0 | `0x40` | rd ← mem[rs1 + imm] | — |
| `STORE` | `STORE imm(rd), rs1` | I | 0 | 2 | 0 | 1 | `0x41` | mem[rd + imm] ← rs1 | — |
| `MOV` | `MOV rd, rs1` | R | 0 | 2 | 0 | 2 | `0x42` | rd ← rs1 | — |
| `LI` | `LI rd, imm` | I | 0 | 2 | 0 | 3 | `0x43` | rd ← imm (zero-extended) | — |
| `LUI` | `LUI rd, imm` | I | 0 | 2 | 0 | 4 | `0x44` | rd ← imm << 16 | — |
| `NOT` | `NOT rd, rs1` | R | 0 | 2 | 1 | 0 | `0x50` | rd ← ~rs1 | Z |
| `NEG` | `NEG rd, rs1` | R | 0 | 2 | 1 | 1 | `0x51` | rd ← −rs1 | Z O C |
| `CMP` | `CMP rd, rs1, rs2` | R | 0 | 3 | 1 | 0 | `0x70` | rd ← (rs1 − rs2 ≠ 0) ? 1 : 0 | Z |
| `ADD` | `ADD rd, rs1, rs2` | R | 0 | 3 | 1 | 1 | `0x71` | rd ← rs1 + rs2 | Z O C |
| `SUB` | `SUB rd, rs1, rs2` | R | 0 | 3 | 1 | 2 | `0x72` | rd ← rs1 − rs2 | Z O C |
| `MUL` | `MUL rd, rs1, rs2` | R | 0 | 3 | 1 | 3 | `0x73` | rd ← low 32 bits of rs1 × rs2 | Z |
| `DIV` | `DIV rd, rs1, rs2` | R | 0 | 3 | 1 | 4 | `0x74` | rd ← rs1 ÷ rs2 | Z |
| `ADDI` | `ADDI rd, rs1, imm` | I | 0 | 3 | 1 | 5 | `0x75` | rd ← rs1 + imm (sign-extended) | Z O C |
| `AND` | `AND rd, rs1, rs2` | R | 0 | 3 | 1 | 9 | `0x79` | rd ← rs1 & rs2 | Z |
| `OR` | `OR rd, rs1, rs2` | R | 0 | 3 | 1 | 10 | `0x7A` | rd ← rs1 \| rs2 | Z |
| `XOR` | `XOR rd, rs1, rs2` | R | 0 | 3 | 1 | 11 | `0x7B` | rd ← rs1 ^ rs2 | Z |
| `SL` | `SL rd, rs1, rs2` | R | 0 | 3 | 1 | 12 | `0x7C` | rd ← rs1 << rs2 (zero-fill) | Z |
| `SR` | `SR rd, rs1, rs2` | R | 0 | 3 | 1 | 13 | `0x7D` | rd ← rs1 >> rs2 (zero-fill) | Z |
| `SLA` | `SLA rd, rs1, rs2` | R | 0 | 3 | 1 | 14 | `0x7E` | rd ← rs1 << rs2 (same as SL) | Z |
| `SRA` | `SRA rd, rs1, rs2` | R | 0 | 3 | 1 | 15 | `0x7F` | rd ← rs1 >> rs2 (sign-fill) | Z |
| `BEQ` | `BEQ rd, rs1, label` | I | 1 | 3 | 1 | 0 | `0xF0` | if rd = rs1: PC ← imm | Z |
| `BNQ` | `BNQ rd, rs1, label` | I | 1 | 3 | 1 | 1 | `0xF1` | if rd ≠ rs1: PC ← imm | Z |
| `BLT` | `BLT rd, rs1, label` | I | 1 | 3 | 1 | 2 | `0xF2` | if rd < rs1: PC ← imm | Z |
| `BGT` | `BGT rd, rs1, label` | I | 1 | 3 | 1 | 3 | `0xF3` | if rd > rs1: PC ← imm | Z |
| `BLE` | `BLE rd, rs1, label` | I | 1 | 3 | 1 | 4 | `0xF4` | if rd ≤ rs1: PC ← imm | Z |
| `BGE` | `BGE rd, rs1, label` | I | 1 | 3 | 1 | 5 | `0xF5` | if rd ≥ rs1: PC ← imm | Z |
