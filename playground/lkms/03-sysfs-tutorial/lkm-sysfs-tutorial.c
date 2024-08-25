// SPDX-License-Identifier: GPL-2.0

// sysfsにオリジナルのディレクトリとファイルを作成するモジュール。
// /sys/fs/kernel/ruth/というディレクトリを作成し、その下に
// nameとstateという2つのファイルを作成する。
// stateから読み出しを行うと、ruth（猫の名前）の現在の状態を
// 取得することができる。stateに"sakana"などの文字列を書き込むと、
// 内部状態が変わるようになっている。

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/printk.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define OURMODNAME "lkm_sysfs_tutorial"

MODULE_AUTHOR("ruth");
MODULE_DESCRIPTION("");
MODULE_LICENSE("Dual MIT/GPL"); // or whatever
MODULE_VERSION("0.1");

static struct kobject *ruth_kobj;

static ssize_t name_show(struct kobject *kobj, struct kobj_attribute *attr,
			 char *buf);
static ssize_t name_store(struct kobject *kobj, struct kobj_attribute *attr,
			  const char *buf, size_t count);

// /sys/kernel/ruth/nameというファイルのattribute
static struct kobj_attribute name_attribute =
	__ATTR(name, 0660, name_show, name_store);

static ssize_t name_show(struct kobject *kobj, struct kobj_attribute *attr,
			 char *buf)
{
	// static変数は0で初期化される
	static int cnt;

	return sprintf(buf, "name! cnt=%d\n", cnt++);
}

static ssize_t name_store(struct kobject *kobj, struct kobj_attribute *attr,
			  const char *buf, size_t count)
{
	return count;
}

// ruthの状態
enum ruth_state {
	RUTH_STATE_NORMAL, // 普通
	RUTH_STATE_SLEEPY, // 眠い
	RUTH_STATE_HAPPY, // 嬉しい
	RUTH_STATE_ANGRY, // 怒っている
	RUTH_STATE_HUNGRY, // お腹がすいている
};

static enum ruth_state ruth_state = RUTH_STATE_NORMAL;

static ssize_t state_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	switch (ruth_state) {
	case RUTH_STATE_NORMAL:
		return sprintf(buf, "...\n");
	case RUTH_STATE_SLEEPY:
		return sprintf(buf, "Zzz..\n");
	case RUTH_STATE_HAPPY:
		return sprintf(buf, "Nyan! Nyan! Nyan!\n");
	case RUTH_STATE_ANGRY:
		return sprintf(buf, "Grurururururu\n");
	case RUTH_STATE_HUNGRY:
		return sprintf(buf, "I'm hungry\n");
	}
}

static ssize_t state_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	static int cnt;

	if (!strcmp("matatabi", buf)) {
		// またたびをもらって幸せになる
		ruth_state = RUTH_STATE_HAPPY;
	} else if (!strcmp("sakana", buf)) {
		// ご飯を食べて眠くなる
		// お腹はいっぱいになったので、カウンタを0に戻す
		ruth_state = RUTH_STATE_SLEEPY;
		cnt = 0;
	} else if (!strcmp("rival", buf)) {
		// ライバルの猫がやってきて怒りだす
		ruth_state = RUTH_STATE_ANGRY;
	} else {
		if (cnt++ < 3) {
			ruth_state = RUTH_STATE_NORMAL;
		} else {
			ruth_state = RUTH_STATE_HUNGRY;
		}
	}
	return count;
}

// /sys/kernel/ruth/stateというファイルのattribute
static struct kobj_attribute state_attribute =
	__ATTR(state, 0660, state_show, state_store);

static int __init lkm_sysfs_tutorial_init(void)
{
	int err;

	// /sys/kernel/ruth/というディレクトリを作成する
	ruth_kobj = kobject_create_and_add("ruth", kernel_kobj);
	if (!ruth_kobj) {
		pr_err("failed to create and add ruth_kobj");
		return -ENOMEM;
	}

	// /sys/kernel/ruth/nameというファイルを作成する
	err = sysfs_create_file(ruth_kobj, &name_attribute.attr);
	if (err) {
		pr_err("failed to create the \"name\" file in /sys/kernel/ruth/");
		return err;
	}

	// /sys/kernel/ruth/stateというファイルを作成する
	err = sysfs_create_file(ruth_kobj, &state_attribute.attr);
	if (err) {
		pr_err("failed to create the \"state\" file in /sys/kernel/ruth/");
		return err;
	}

	return 0;
}

static void __exit lkm_sysfs_tutorial_exit(void)
{
	// 作成したkobjectを削除する
	// kobject_putは参照カウンタをデクリメントし、もし0になったら
	// システムからkobjを削除する、といった使い方をする。
	kobject_put(ruth_kobj);
}

module_init(lkm_sysfs_tutorial_init);
module_exit(lkm_sysfs_tutorial_exit);
