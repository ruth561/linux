/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_RUTH_H
#define _LINUX_RUTH_H

#include <linux/bpf.h>


int ruth_prog_attach(const union bpf_attr *attr, struct bpf_prog *prog);
int ruth_prog_detach(const union bpf_attr *attr, struct bpf_prog *prog);

#endif /* _LINUX_RUTH_H */
