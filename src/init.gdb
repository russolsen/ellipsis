file kernel.elf
target remote :1234
b repl
b CLisp::Eval
dis 2
cont

