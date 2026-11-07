1. First pull in and expand the submodule and it's submodules.

git submodule update --init --recursive

2. Next build circle with a US default keyboad

cd circle
./configure  -d DEFAULT_KEYMAP='\"US\"'
./makeall 

3. And build the fatfs addon

cd circle/addon/fatfs
make

4. Build the sdcard addon

cd circle/addon/SDCard
make
