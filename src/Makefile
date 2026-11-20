#
# Makefile
#

TOP := ..
CIRCLEHOME = $(TOP)/circle

FATFS=$(CIRCLEHOME)/addon/fatfs

PRIMS   = prims_basic.o prims_io.o

OBJS	= hcireader.o bufferedreader.o printfer.o utils.o value.o ellipsis.o kernel.o repl.o fatfiledevice.o stringdevice.o lisp.o parser.o manager.o  prompter.o $(PRIMS)

LIBS	= $(FATFS)/libfatfs.a \
	  $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a

default: kernel.img _startup.txt run

all: kernel.img

include $(CIRCLEHOME)/sample/Rules.mk

_startup.txt: startup.lsp
	mkdir -p sd
	sudo mount $(TOP)/sdcard.img sd
	sudo cp $< sd
	sudo umount sd
	date > $@
	
run: kernel.img
	qemu-system-arm -M raspi0 -device usb-kbd -k en -serial stdio  -drive if=sd,driver=file,filename=$(TOP)/sdcard.img -bios kernel.img


-include $(DEPS)
