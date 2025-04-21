
.PHONY: config build clear rebuild

ROOT=$(shell pwd)
DEMO_PATH=E:\Project\STM32\F103CBT6\HelloWorld\cmake-build-debug-mingw-stm32\F103CBT6.elf

run:
	${ROOT}/build/qemu-system-ricky.exe -M RickyBoard -kernel ${DEMO_PATH} -monitor stdio -d in_asm


build: clear
	make -j14


config: clear
	./configure --target-list=ricky-softmmu \
	--enable-debug \
	--enable-gtk


clear:
	clear

rebuild:
	rm -rf /build/*
	make -f doit.mk config
	make -f doit.mk build

