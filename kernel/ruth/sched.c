/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  kernel/ruth/sched.c
 *
 *  Kernel Scheduler Prober
 *
 *  Copyright (C) 2024 ruth  
 */

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/sched/clock.h>
#include <linux/sysfs.h>
#include <linux/printk.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/percpu.h>
#include <linux/kobject.h>
#include <linux/ruth/sched.h>


static DEFINE_PER_CPU(u64, cnt___schedule_entry) = 0;
static DEFINE_PER_CPU(u64, cnt___schedule_exit) = 0;
static DEFINE_PER_CPU(u64, timestamp___schedule) = 0;
static DEFINE_PER_CPU(u64, exec_time___schedule) = 0;

void ruth_hook___schedule_entry(void)
{
	this_cpu_inc(cnt___schedule_entry);
	this_cpu_write(timestamp___schedule, sched_clock());
}

void ruth_hook___schedule_exit(void)
{
	u64 elapsed_time;

	this_cpu_inc(cnt___schedule_exit);
	elapsed_time = sched_clock() - this_cpu_read(timestamp___schedule);
	this_cpu_add(exec_time___schedule, elapsed_time);
}

// Implementation of /sys/kernel/ruth dir

// the structure of /sys/kernel/ruth dir
struct kobject *ruth_kobj;

static ssize_t sched_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	int cpu;
	u64 entries, exits, exec_time;

	preempt_disable();
	cpu = smp_processor_id();
	entries = this_cpu_read(cnt___schedule_entry);
	exits = this_cpu_read(cnt___schedule_exit);
	exec_time = this_cpu_read(exec_time___schedule);
	preempt_enable();

	return sprintf(buf,
		"cpu: %d\n"
		"__schedule entry:     %lld\n"
		"__schedule exit:      %lld\n"
		"__schedule exec time: %lld\n"
		"__schedule avg time:  %lld\n",
		cpu, entries, exits, exec_time, exec_time / exits
	);
}

// the structure of the files in /sys/kernel/ruth dir
static struct kobj_attribute sched_attribute =
	__ATTR(sched, 0440, sched_show, NULL);

static int __init init_ruth_dir(void)
{
	int err;

	ruth_kobj = kobject_create_and_add("ruth", kernel_kobj);
	if (!ruth_kobj) {
		pr_err("Failed to creat or add /sys/kernel/ruth\n");
		return -ENOMEM;
	}

	err = sysfs_create_file(ruth_kobj, &sched_attribute.attr);
	if (err) {
		pr_err("Failed to creat /sys/kernel/ruth/sched\n");
		return err;
	}
	return 0;
}
postcore_initcall(init_ruth_dir);
