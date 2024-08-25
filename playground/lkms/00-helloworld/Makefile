PWD	:= $(shell pwd)
obj-m	+= helloworld-lkm.o


all:
	make CC=clang -C ../../../ M=$(PWD) modules

qemu:
	make all
	cp helloworld-lkm.ko ../../rootfs/misc/
	cd ../../; make qemu

clean:
	make CC=clang -C ../../../ M=$(PWD) clean
