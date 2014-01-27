/*
 * LiMiT -- LightweIght MonItoring Toolkit CPP Utility Library
 *   http://castl.cs.columbia.edu/limit
 *
 *   A userland performance counter-based counter profiler.  Allows
 * users with to configure performance counters and read them.  Also
 * provides a variety of userspace utilities to assist in
 * configuration, reading and accounting.
 *
 *    Copyright 2010, John Demme, Columbia University
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

#include <limit.h>
#include <cstdio>

/*
 * These classes are intended to provide easy ways to instrument code
 * blocks and accumulate totals for said blocks.  Timer uses C++
 * constructor and destructor semantics, so make sure to place them on
 * the stack -- do not heap allocate them.  struct Limit is most
 * effective as a global variable so it initializes and prints results
 * at startup and teardown of your application.
 */

void limit_cpp_thread(char, void*);
struct Limit {
    uint64_t parent_start;
    volatile uint64_t count;
    volatile uint64_t cumulative;

    Limit(uint64_t event) : count(0), cumulative(0) {
	lprof_thread_notify(&limit_cpp_thread, this);
	lprof_init(1, event);
	lprof(1, parent_start);
    }

    ~Limit() {
	uint64_t end;
	lprof(1, end);
	cumulative += end - parent_start;

	printf("LiMiT\tTotal: %lu\n\tRegion: %lu\n\tPcnt: %lf%%\n",
	       cumulative, count, ((double)count*100) / cumulative);
    }
};

extern __thread char inTimer;
class Timer {
    Limit* limit;
    uint64_t start;

public:
    Timer(Limit& l) : limit(&l) {
	inTimer++;
	lprof(1, start);
    }
    
    ~Timer() {
	uint64_t end;
	lprof(1, end);
	uint64_t delta = (end - start);
	inTimer--;
	if (inTimer == 0)
	    __sync_fetch_and_add(&limit->count, delta);
	//limit->count += delta;
    }
};

