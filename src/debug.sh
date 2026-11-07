#!/bin/bash

#gdb -ex "file kernel.elf" -ex "target remote :1234"  -ex "b Repl" -ex cont

gdb -x init.gdb
        
