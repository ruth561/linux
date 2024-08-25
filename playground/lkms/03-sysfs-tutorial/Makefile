# TODO: ここにモジュール名（.cを除いたもの）を設定する
MODULE_NAME	:= lkm-sysfs-tutorial

PWD		:= $(shell pwd)
src		:= $(MODULE_NAME).c
target		:= $(MODULE_NAME).ko
obj-m		+= $(MODULE_NAME).o

EXTRA_CFLAGS	+= -DDEBUG


all: $(target)

$(target): $(src)
	make CC=clang -C ../../../ M=$(PWD) modules

qemu: $(target)
	cp $(target) ../../rootfs/misc/
	cd ../../; make qemu 

clean:
	make CC=clang -C ../../../ M=$(PWD) clean
# `make code-style`で生成されるファイルを削除
	rm -f *~

# Linuxカーネルのコード規約に遵守するようにコードを整形する
code-style:
	make clean
	../../../scripts/checkpatch.pl --no-tree -f --max-line-length=95 *.[ch]
