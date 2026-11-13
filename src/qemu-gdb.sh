#/bin/bash

# For some reason qemu is only happy if it's turn in the background.

qemu-system-arm -s -S -M raspi0 -device usb-kbd -k en -serial stdio  \
                -drive if=sd,driver=file,filename=../sdcard.img -bios kernel.img &
        
