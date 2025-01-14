// Copyright 2013 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// +build solaris

#include "config.h"

#include <errno.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include "runtime.h"
#include "malloc.h"

static Lock selectlock;
static int rdwake;
static int wrwake;
static fd_set fds;
static PollDesc **data;
static int allocated;

void
runtime_netpollinit(void)
{
	int p[2];
	int fl;

	FD_ZERO(&fds);
	allocated = 128;
	data = runtime_mallocgc(allocated * sizeof(PollDesc *), 0,
				FlagNoScan|FlagNoProfiling|FlagNoInvokeGC);

	if(pipe(p) < 0)
		runtime_throw("netpollinit: failed to create pipe");
	rdwake = p[0];
	wrwake = p[1];

	fl = fcntl(rdwake, F_GETFL);
	if(fl < 0)
		runtime_throw("netpollinit: fcntl failed");
	fl |= O_NONBLOCK;
	if(fcntl(rdwake, F_SETFL, fl))
		 runtime_throw("netpollinit: fcntl failed");
	fcntl(rdwake, F_SETFD, FD_CLOEXEC);

	fl = fcntl(wrwake, F_GETFL);
	if(fl < 0)
		runtime_throw("netpollinit: fcntl failed");
	fl |= O_NONBLOCK;
	if(fcntl(wrwake, F_SETFL, fl))
		 runtime_throw("netpollinit: fcntl failed");
	fcntl(wrwake, F_SETFD, FD_CLOEXEC);

	FD_SET(rdwake, &fds);
}

int32
runtime_netpollopen(uintptr fd, PollDesc *pd)
{
	byte b;

	runtime_lock(&selectlock);

	if((int)fd >= allocated) {
		int c;
		PollDesc **n;

		c = allocated;

		runtime_unlock(&selectlock);

		while((int)fd >= c)
			c *= 2;
		n = runtime_mallocgc(c * sizeof(PollDesc *), 0,
				     FlagNoScan|FlagNoProfiling|FlagNoInvokeGC);

		runtime_lock(&selectlock);

		if(c > allocated) {
			__builtin_memcpy(n, data, allocated * sizeof(PollDesc *));
			allocated = c;
			data = n;
		}
	}
	FD_SET(fd, &fds);
	data[fd] = pd;

	runtime_unlock(&selectlock);

	b = 0;
	write(wrwake, &b, sizeof b);

	return 0;
}

int32
runtime_netpollclose(uintptr fd)
{
	byte b;

	runtime_lock(&selectlock);

	FD_CLR(fd, &fds);
	data[fd] = nil;

	runtime_unlock(&selectlock);

	b = 0;
	write(wrwake, &b, sizeof b);

	return 0;
}

G*
runtime_netpoll(bool block)
{
	fd_set rfds, wfds, efds, tfds;
	struct timeval timeout;
	struct timeval *pt;
	int max, c, i;
	G *gp;
	int32 mode;
	byte b;
	struct stat st;

 retry:
	runtime_lock(&selectlock);

	max = allocated;

	if(max == 0) {
		runtime_unlock(&selectlock);
		return nil;
	}

	__builtin_memcpy(&rfds, &fds, sizeof fds);

	runtime_unlock(&selectlock);

	__builtin_memcpy(&wfds, &rfds, sizeof fds);
	FD_CLR(rdwake, &wfds);
	__builtin_memcpy(&efds, &wfds, sizeof fds);

	__builtin_memcpy(&tfds, &wfds, sizeof fds);

	__builtin_memset(&timeout, 0, sizeof timeout);
	pt = &timeout;
	if(block)
		pt = nil;

	c = select(max, &rfds, &wfds, &efds, pt);
	if(c < 0) {
		if(errno == EBADF) {
			// Some file descriptor has been closed.
			// Check each one, and treat each closed
			// descriptor as ready for read/write.
			c = 0;
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_ZERO(&efds);
			for(i = 0; i < max; i++) {
				if(FD_ISSET(i, &tfds)
				   && fstat(i, &st) < 0
				   && errno == EBADF) {
					FD_SET(i, &rfds);
					FD_SET(i, &wfds);
					c += 2;
				}
			}
		}
		else {
			if(errno != EINTR)
				runtime_printf("runtime: select failed with %d\n", errno);
			goto retry;
		}
	}
	gp = nil;
	for(i = 0; i < max && c > 0; i++) {
		mode = 0;
		if(FD_ISSET(i, &rfds)) {
			mode += 'r';
			--c;
		}
		if(FD_ISSET(i, &wfds)) {
			mode += 'w';
			--c;
		}
		if(FD_ISSET(i, &efds)) {
			mode = 'r' + 'w';
			--c;
		}
		if(i == rdwake) {
			while(read(rdwake, &b, sizeof b) > 0)
				;
			continue;
		}
		if(mode) {
			PollDesc *pd;

			runtime_lock(&selectlock);
			pd = data[i];
			runtime_unlock(&selectlock);
			if(pd != nil)
				runtime_netpollready(&gp, pd, mode);
		}
	}
	if(block && gp == nil)
		goto retry;
	return gp;
}

void
runtime_netpoll_scan(void (*addroot)(Obj))
{
	addroot((Obj){(byte*)&data, sizeof data, 0});
}
