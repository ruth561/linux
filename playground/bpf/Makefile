# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the bpf program for BPF_PROG_TYPE_RUTH
#

target	:= ruth
src	:= $(target).c
PWD	:= $(shell pwd)
CC	:= clang
CFLAGS	:= -Wall --target=x86_64-linux-gnu -Wall -std=gnu11
CFLAGS	+= -static
CFLAGS	+= -Iinclude


$(target): ruth.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: qemu
qemu: $(target)
	cp ruth ../rootfs/misc/
	cd ..; make qemu

.PHONY: clean
clean:
	rm $(target)

kernel-headers:
	cd ../..; make headers_install INSTALL_HDR_PATH=$(PWD)

# TODO: まだ、ruth.cに対するエントリしか対応できていない
#       ゆくゆくは、BPFプログラムを増やしていったときに、すべての
#       エントリを出力できるようにしたい。
.PHONY: gen-entry-for-compile-commands.json
gen-entry-for-compile-commands.json:
	@$(CC) $(CFLAGS) -MJ /dev/stdout -o $(target) $(src)
