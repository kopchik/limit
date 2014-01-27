/*
 * LiMiT
 *
 *   A userland performance counter-based counter profiler.  Allows
 * users with to configure performance counters and read them.  Also
 * provides a variety of userspace utilities to assist in
 * configuration, reading and accounting.
 *
 *    Copyright (C) 2010, John Demme, Columbia University
 *
 *  LiMiT is free software: you can redistribute it and/or modify it
 *  under the terms of the Lesser GNU General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  LiMiT is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with LiMiT.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE
#include <limit-cpp.h>
#include <cstdio>
#include <unistd.h>
#include <linux/unistd.h>

__thread char inTimer = 0;

static pid_t gettid(void) {
    return syscall(__NR_gettid);
}


static __thread uint64_t start;
void limit_cpp_thread(char status, void* l) {
    Limit* limit = (Limit*)l;
    if (status == 0) {
	lprof(1, start);
	printf("(%u) Born\n", gettid());
    } else if (status == 1) {
	uint64_t end;
	lprof(1, end);
	limit->cumulative += end - start;
	printf("(%u) Adding %lu\n", gettid(), end - start);
    }
}
