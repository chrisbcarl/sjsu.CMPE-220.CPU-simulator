feature: isa ADD
    MODIFY:
    fetch
    decode
    execute
    mem
    wb

    MODIFY:
    isa_is_valid
    isa_decode
    isa_encode
    isa_mnemonic
    isa_disassemble
    instructions file
    test cases

unrelated
    linter ci/cd?

phases
    - simple
        diagram (data path)
        sample code
            compiled bytecode
        ISA w/ opcodes
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
