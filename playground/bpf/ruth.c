// SPDX-License-Identifier: GPL-2.0-only
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <asm/unistd.h>
#include <linux/filter.h>
#include <linux/bpf.h>

typedef unsigned long long u64;
typedef unsigned int u32;


// 32-bitの即値をレジスタにセットする命令
// 疑似コード
//     DST := IMM
#define BPF_MOV32_IMM(DST, IMM)					\
	((struct bpf_insn) {					\
		.code = BPF_ALU | BPF_MOV | BPF_K,		\
		.dst_reg = DST,					\
		.src_reg = 0,					\
		.off   = 0,					\
		.imm   = IMM })

// BPFプログラムの実行を終了する命令
#define BPF_EXIT_INSN()						\
	((struct bpf_insn) {					\
		.code  = BPF_JMP | BPF_EXIT,			\
		.dst_reg = 0,					\
		.src_reg = 0,					\
		.off   = 0,					\
		.imm   = 0 })

// bpf()システムコールのラッパー関数
static int sys_bpf(enum bpf_cmd cmd, union bpf_attr *attr, unsigned int size)
{
	return syscall(__NR_bpf, cmd, attr, size);
}

// bpf(BPF_PROG_LOAD, ...)システムコールを実行し、BPFプログラムをカーネルに
// ロードする。ロードに成功したら、対応するfdを返し、失敗したら負の値を返す。
static int sys_bpf_prog_load(u32 prog_type, u32 expected_attach_type,
			     const char *license, struct bpf_insn *insns,
			     size_t insn_cnt, const char *name)
{
	union bpf_attr attr;
	// union bpf_attrは様々なデータ構造をひとまとめにしたもの。
	// そのため、cmdの違いによってattrのサイズも変わってくる。
	// BPF_PROG_LOADのときの一番うしろのフィールドの名前は
	// prog_token_fdなので、そこまでのオフセットとそのフィールドのサイズ
	// の合計が、attrのサイズとなる。
	unsigned int attr_size = offsetof(union bpf_attr, prog_token_fd) + sizeof(attr.prog_token_fd);
	
	memset(&attr, 0, attr_size);
	attr.prog_type = prog_type;
	attr.expected_attach_type = expected_attach_type;
	attr.license = (u64) license;
	attr.insn_cnt = insn_cnt;
	attr.insns = (u64) insns;
	strncpy(attr.prog_name, name, sizeof(attr.prog_name) - 1); // 最後の1-byteはNULL終端

	return sys_bpf(BPF_PROG_LOAD, &attr, attr_size);
}

// bpf(BPF_PROG_ATTACH, ...)システムコールを実行し、BPFプログラムをカーネル
// 内のイベントにattachする。attach_bpf_fdには、すでにロード済みのBPFプログラム
// へのfdを指定する（sys_bpf_prog_loadで返された値）。
// attachに成功したら0を返す？
static int sys_bpf_prog_attach(u32 attach_type, int attach_bpf_fd)
{
	union bpf_attr attr;
	// sys_bpf_prog_loadの説明を参照
	unsigned int attr_size = offsetof(union bpf_attr, expected_revision) + sizeof(attr.expected_revision);

	// attr.target_fdにはattachする先のhookのfdを指定（0でいいらしい）？
	// attr.attach_bpf_fdにはattachするBPFプログラムのfdを指定する
	memset(&attr, 0, attr_size);
	attr.attach_bpf_fd = attach_bpf_fd;
	attr.attach_type = attach_type;

	return sys_bpf(BPF_PROG_ATTACH, &attr, attr_size);
}

static int ruth_load(struct bpf_insn *insns, size_t insn_cnt, const char *name)
{
	return sys_bpf_prog_load(BPF_PROG_TYPE_RUTH, BPF_RUTH, "GPL", insns, insn_cnt, name);
}

static int ruth_attach(int attach_bpf_fd)
{
	return sys_bpf_prog_attach(BPF_RUTH, attach_bpf_fd);
}

int main(void)
{
	int fd, err;

	// 常に42を返すBPFプログラム
	struct bpf_insn insns[] = {
		// BPF_LDX_MEM(BPF_W, BPF_REG_0, BPF_REG_1, 0),
		// BPF_ALU32_IMM(BPF_ADD, BPF_REG_0, 42),
		BPF_MOV32_IMM(BPF_REG_0, 42),
		BPF_EXIT_INSN(),
	};
	size_t insn_cnt = sizeof(insns) / sizeof(insns[0]);

	fd = ruth_load(insns, insn_cnt, "ruth42");
	if (fd < 0) {
		printf("failed to load bpf program. errno=%d.\n", errno);
		return fd;
	}
	printf("Successfully load bpf program! fd=%d.\n", fd);

	err = ruth_attach(fd);
	if (err < 0) {
		printf("failed to attach bpf program. errno=%d.\n", errno);
		return err;
	}
	printf("Successfully attach bpf program!\n");

	return 0;
}
