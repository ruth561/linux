# SPDX-License-Identifier: GPL-2.0

PWD		:= $(shell pwd)
KERNEL_BZIMAGE	:= $(PWD)/../arch/x86_64/boot/bzImage

QEMU		:= qemu-system-x86_64

QEMU_ARGS	:= -kernel $(KERNEL_BZIMAGE)
QEMU_ARGS	+= -initrd rootfs.cpio
QEMU_ARGS	+= -nographic
QEMU_ARGS	+= -append "console=ttyS0 loglevel=3 nokaslr"
QEMU_ARGS	+= -s


qemu:
	$(MAKE) rootfs
	$(QEMU) $(QEMU_ARGS)

.PHONY: rootfs
rootfs:
	cd rootfs; find . -print0 | cpio --null -o --format=newc > ../rootfs.cpio; cd ..