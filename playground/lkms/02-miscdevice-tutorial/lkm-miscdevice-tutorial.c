// SPDX-License-Identifier: GPL-2.0

// /dev/ruthというMISCデバイスファイルを作成するモジュール。

#include "asm-generic/errno-base.h"
#include "linux/dev_printk.h"
#include "linux/device.h"
#include "linux/printk.h"
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ruth");
MODULE_DESCRIPTION("simplest implementation of miscdevice");

#define DEVICE_NAME "ruth"

static int ruth_open(struct inode *inode, struct file *filp);
static ssize_t ruth_read(struct file *filp, char __user *ubuf, size_t count,
		  loff_t *off);
static ssize_t ruth_write(struct file *filp, const char __user *ubuf, size_t count,
		   loff_t *off);
static int ruth_release(struct inode *inode, struct file *filp);

static const struct file_operations ruth_fops = {
	.open = ruth_open,
	.read = ruth_read,
	.write = ruth_write,
	.release = ruth_release,
};

static struct miscdevice ruth_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &ruth_fops,
	.mode = 0666,
};

static int ruth_open(struct inode *inode, struct file *filp)
{
	return nonseekable_open(inode, filp);
}

static ssize_t ruth_read(struct file *filp, char __user *ubuf, size_t count,
		  loff_t *off)
{
#define BUF_SIZE 9
	char buf[] = "AAAAAAAA";

	count = count < 9 ? count : 9;

	if (copy_to_user(ubuf, buf, count)) {
		pr_warn("copy_to_user() failed");
		return -EFAULT;
	}
	return count;
}

static ssize_t ruth_write(struct file *filp, const char __user *ubuf, size_t count,
		   loff_t *off)
{
	char buf[0x10];

	count = count < 0x10 ? count : 0x10;

	pr_info("write(%p, %d, %d) occurred", ubuf, count, off);
	if (copy_from_user(buf, ubuf, 0x10)) {
		pr_warn("copy_from_user() failed");
		return -EFAULT;
	}
	pr_info("write %s", buf);
	return count;
}

static int ruth_release(struct inode *inode, struct file *filp)
{
	pr_info("close /dev/ruth");
	return 0;
}

static int __init miscdevice_tutorial_init(void)
{
	int ret;
	struct device *dev;

	ret = misc_register(&ruth_misc);
	if (ret) {
		pr_err("failed to register ruth_misc");
		return ret;
	}

	dev = ruth_misc.this_device;
	pr_info("Successfully registered ruth device file (minor #%d",
		ruth_misc.minor);
	dev_info(dev, "ruth device");

	pr_info("miscdevice initialized!!");
	return 0;
}

static void __exit miscdevice_tutorial_exit(void)
{
	pr_info("miscdevice cleanup.");
}

module_init(miscdevice_tutorial_init);
module_exit(miscdevice_tutorial_exit);
