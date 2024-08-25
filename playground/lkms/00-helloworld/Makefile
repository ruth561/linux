PWD	:= $(shell pwd)
obj-m	+= lkm-helloworld.o


all:
	make CC=clang -C ../../../ M=$(PWD) modules

qemu:
	make all
	cp lkm-helloworld.ko ../../rootfs/misc/
	cd ../../; make qemu

clean:
	make CC=clang -C ../../../ M=$(PWD) clean
