file kernel.elf
target remote :1234
b CRepl::Run
b CLisp::Eval
dis 2
cont

