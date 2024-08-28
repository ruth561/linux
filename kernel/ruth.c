// SPDX-License-Identifier: GPL-2.0-only

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/filter.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/bpf.h>
#include <linux/ruth.h>

bool ruth_is_valid_access(int off, int size, enum bpf_access_type type,
			  const struct bpf_prog *prog,
			  struct bpf_insn_access_aux *info);

const struct bpf_func_proto *ruth_get_func_proto(enum bpf_func_id func_id,
						 const struct bpf_prog *prog);

const struct bpf_verifier_ops ruth_verifier_ops = {
	.get_func_proto = ruth_get_func_proto,
	.is_valid_access = ruth_is_valid_access,
};

const struct bpf_prog_ops ruth_prog_ops = {};

// ============================================== //
//                 eBPF実装                       //
// ============================================== //

// ctx領域へのアクセスが有効であればtrueを返す。
// BPF_PROG_TYPE_RUTHでは、ctx領域は以下のようになっている。
//         |-------------|
// ctx --> |   s32 num   |
//         |-------------|
bool ruth_is_valid_access(int off, int size, enum bpf_access_type type,
			  const struct bpf_prog *prog,
			  struct bpf_insn_access_aux *info)
{
	if (off < 0 || off + size > sizeof(s32))
		return false;
	return true;
}

// BPF_PROG_TYPE_RUTHでは、基本的なヘルパー関数のみ使えるようにする。
const struct bpf_func_proto *ruth_get_func_proto(enum bpf_func_id func_id,
						 const struct bpf_prog *prog)
{
	return bpf_base_func_proto(func_id, prog);
}

// ============================================== //
//   kernel/bpf/syscall.cから呼び出される処理     //
// ============================================== //

struct bpf_prog *ruth_prog;

// bpf(BPF_PROG_ATTACH, ...)システムコールが発行されたときに呼び出される関数。
int ruth_prog_attach(const union bpf_attr *attr, struct bpf_prog *prog)
{
	if (attr->attach_type != BPF_RUTH)
		return -EINVAL;
	ruth_prog = prog;
	return 0;
}

// bpf(BPF_PROG_DETACH, ...)システムコールが発行されたときに呼び出される関数。
int ruth_prog_detach(const union bpf_attr *attr, struct bpf_prog *prog)
{
	if (attr->attach_type != BPF_RUTH)
		return -EINVAL;
	ruth_prog = NULL;
	return 0;
}

// ============================================== //
//         /sys/kernel/ruth/num周りの処理         //
// ============================================== //

// /sys/kernel/ruthのkobject
struct kobject *ruth_kobj;
// numに書き込まれた値を保存するためのグローバル変数
s32 gnum;

// /sys/kernel/ruth/numファイルが読み出されたときのコールバック。
static ssize_t num_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	pr_info("num_show() called!\n");
	return sprintf(buf, "%d\n", gnum);
}

// /sys/kernel/ruth/numファイルに書き込みが行われたときのコールバック。
static ssize_t num_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int err, var;

	pr_info("num_store() called!\n");
	// 入力文字列をintに変換する
	err = kstrtoint(buf, 10, &var);
	if (err) {
		pr_err("failed to kstrtoint\n");
		return err;
	}
	pr_info("var = %d\n", var);

	if (ruth_prog) {
		// BPFプログラムがセットされていたら
		gnum = bpf_prog_run(ruth_prog, &var);
	} else {
		// BPFプログラムがセットされていなかったら
		gnum = -1;
	}
	return count;
}

// /sys/kernel/ruth/numというファイルのattribute
static struct kobj_attribute num_attribute =
	__ATTR(num, 0660, num_show, num_store);

static int __init init_ruth(void)
{
	int err;

	pr_info("init_ruth called!!!\n");

	// /sys/kernel/ruthというディレクトリを作成
	ruth_kobj = kobject_create_and_add("ruth", kernel_kobj);
	if (!ruth_kobj) {
		pr_err("failed to init ruth...\n");
		return -ENOMEM;
	}

	// /sys/kernel/ruthにnumというファイルを作成
	err = sysfs_create_file(ruth_kobj, &num_attribute.attr);
	if (err) {
		pr_err("failed to create the \"num\" file in /sys/kernel/ruth/\n");
		return err;
	}

	return 0;
}
postcore_initcall(init_ruth);
