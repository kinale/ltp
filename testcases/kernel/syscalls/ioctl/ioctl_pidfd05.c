// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2025 Andrea Cervesato <andrea.cervesato@suse.com>
 */

/*\
 * Verify that ioctl() raises an EINVAL error when PIDFD_GET_INFO is used. This
 * happens when:
 *
 * - info parameter is NULL
 * - info parameter is providing the wrong size
 */

#include "tst_test.h"
#include "lapi/pidfd.h"
#include "lapi/sched.h"
#include "lapi/ioctl.h"

struct pidfd_info_invalid {
	uint32_t dummy;
};

#define PIDFD_GET_INFO_SHORT _IOWR(PIDFS_IOCTL_MAGIC, 11, struct pidfd_info_invalid)

static struct tst_clone_args *args;
static struct pidfd_info_invalid *info_invalid;

static void run(void)
{
	int pidfd = 0;
	pid_t pid_child;

	memset(args, 0, sizeof(struct tst_clone_args));

	info_invalid->dummy = 1;

	args->flags = CLONE_PIDFD | CLONE_NEWUSER | CLONE_NEWPID;
	args->pidfd = (uint64_t)&pidfd;
	args->exit_signal = SIGCHLD;

	pid_child = SAFE_CLONE(args);
	if (!pid_child)
		exit(0);

	TST_EXP_FAIL(ioctl(pidfd, PIDFD_GET_INFO, NULL), EINVAL);
	TST_EXP_FAIL(ioctl(pidfd, PIDFD_GET_INFO_SHORT, info_invalid), EINVAL);

	SAFE_CLOSE(pidfd);
}

static struct tst_test test = {
	.test_all = run,
	.forks_child = 1,
	.bufs = (struct tst_buffers []) {
		{&args, .size = sizeof(*args)},
		{&info_invalid, .size = sizeof(*info_invalid)},
		{}
	}
};
