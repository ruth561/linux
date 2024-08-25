// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define OURMODNAME   "lkm_template"

MODULE_AUTHOR("Kaiwan N Billimoria");
MODULE_DESCRIPTION("a simple LKM template; do refer to the (better) Makefile as well");
MODULE_LICENSE("Dual MIT/GPL");	// or whatever
MODULE_VERSION("0.1");

static int __init lkm_template_init(void)
{
	pr_info("inserted\n");
	return 0;		/* success */
}

static void __exit lkm_template_exit(void)
{
	pr_info("removed\n");
}

module_init(lkm_template_init);
module_exit(lkm_template_exit);
