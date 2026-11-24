FIX:
    - void pointer casting
        - all instrs have same interface?? just dont use all
        - funcs in isa.c as void pointers? cast later?
            void (*itype)(uint32_t* rd, uint16_t* imm);
            void (*binary)(uint32_t* rd, uint32_t* rs1, uint8_t* flags);
            void (*trinary)(uint32_t* rd, uint32_t* rs1, uint32_t* rs2, uint8_t* flags);



unrelated
    linter ci/cd?

phases
    - instruction implementation
    - stretch
        logic
        l2 cache (RAM)
        forwarding
        functional units, superscaling
        compiler

Contributors
- chrisbcarl, chriscarl-sjsu
- harshakrishna15
- ReverseBack2
- jehuzepeda-silva-rgb
- Bageldorf
- sriharsha8000
