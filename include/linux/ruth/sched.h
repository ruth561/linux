/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_RUTH_SCHED_H
#define _LINUX_RUTH_SCHED_H

// __scheduleの先頭と最後に呼び出されるフック関数
void ruth_hook___schedule_entry(void);
void ruth_hook___schedule_exit(void);

#endif /* _LINUX_RUTH_SCHED_H */
