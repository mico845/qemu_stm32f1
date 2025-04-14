
.PHONY: config build clear

ROOT=$(shell pwd)
DEMO_PATH=${ROOT}/demo

run:
#	cd ${DEMO_PATH} && ${ROOT}/build/DSP6657.exe -atsConfig ${DEMO_PATH}/conf6657.json -monitor stdio -vnc :0 -D dsp6657.log -devlog TIMER -S -s
# -d in_asm,op,exec
# -monitor vc:800x600

build: clear
	make -j14


config: clear
	./configure --target-list=ricky-softmmu --enable-debug \
	--disable-install-blobs --disable-guest-agent --disable-sdl \
	--disable-curses --disable-werror --enable-tcg-interpreter

clear:
	clear