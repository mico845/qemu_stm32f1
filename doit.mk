
.PHONY: config build clear

ROOT=$(shell pwd)
DEMO_PATH=E:\Project\STM32\F103CBT6\HelloWorld\cmake-build-debug-mingw-stm32\F103CBT6.bin

run:
	${ROOT}/build/qemu-system-ricky.exe -M ricky -kernel ${DEMO_PATH} -monitor stdio -d in_asm


build: clear
	make -j14


config: clear
	./configure --target-list=ricky-softmmu --enable-debug \
	--disable-install-blobs --disable-guest-agent --enable-gtk \
	--disable-curses --disable-werror --enable-tcg-interpreter

clear:
	clear