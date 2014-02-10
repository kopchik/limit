/*
 * LiMiT -- LightweIght MonItoring Toolkit
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

#ifndef _LPROF_H
#define _LPROF_H

#ifndef __LP_ASM_ONLY
#include <unistd.h>
#include <stdint.h>

#define MAX_COUNTERS  64

// microarch neutral
#define EV_CYCLES	   (0x3C | (0x0 << 8))
#define EV_REF_CYCLES	   (0x3C | (0x1 << 8))
#define EV_INSTR	   (0xC0 | (0x0 << 8))
#define EV_BRANCH	   (0xC4 | (0x1 << 8))
#define EV_BRANCH_MISS     (0xC5 | (0x1 << 8))

// microarch specific
#define I7_L3_REFS	(0x2e | (0x4f << 8))
#define I7_L3_MISS	(0x2e | (0x41 << 8))

#define I7_L2_REFS	(0x24 | (0xff << 8))
#define I7_L2_MISS	(0x24 | (0xaa << 8))

#define I7_ICACHE_HITS  (0x80 | (0x01 << 8))
#define I7_ICACHE_MISS  (0x80 | (0x02 << 8))

#define I7_DL1_REFS     (0x43 | (0x01 << 8))

#define I7_LOADS        (0x0b | (0x01 << 8))
#define I7_STORES       (0x0b | (0x02 << 8))

#define I7_L2_DTLB_MISS (0x49 | (0x01 << 8))
#define I7_L2_ITLB_MISS (0x85 | (0x01 << 8))

#define I7_IO_TXNS      (0x6c | (0x01 << 8))
#define I7_DRAM_REFS    (0x0f | (0x20 << 8))

#define PERFMON_EVENTSEL_INV    (1 << 23)
#define PERFMON_EVENTSEL_ENABLE (1 << 22)
#define PERFMON_EVENTSEL_EDGE   (1 << 18)
#define PERFMON_EVENTSEL_OS     (1 << 17)
#define PERFMON_EVENTSEL_USR    (1 << 16)

#ifdef __cplusplus
#include <string>
unsigned int get_event(std::string name);

 extern "C" {
#endif

extern __thread struct lprof_stats __lp_stats;


// Initialize LiMiT
int lprof_init(unsigned int num, ...);
// Close LiMiT
void lprof_close();
// Print some debug information
void lprof_debug(void);

// LiMiT must be notified when new threads are created
typedef void (*thread_notif)(char, void*);
void lprof_thread_notify(thread_notif notif, void* data);

// Bring in stuff from the kernel patch for machines without it or
// people too lazy to -I the kernel include dir.
#ifndef _LINUX_LPROF_H
#define MAX_PMCS   8

struct lprof_stats {
    unsigned char rdpmc_inprogress; /* Offset: 0 bytes */
    uint64_t context_switches;	    /* Offset: 8 byte */
    int64_t last[MAX_PMCS];	    /* Offsets: [0]: 16
				                [1]: 24 
						[2]: 32 ...*/
};
#endif //!_LINUX_LPROF_H

#define RDX %%rdx
#define RAX %%rax
#define FS  %%fs

#endif //!__LP_ASM_ONLY

#define __QUOTEME_(...) #__VA_ARGS__
#define __QUOTEME(x) __QUOTEME_(x)
#define __LP_OFF(C) FS:__lp_stats@TPOFF+16+(C)*8

#define __LP_ASM(C)				\
     rdpmc;					\
     shl $32, RDX;				\
     orq RAX, RDX;      			\
     subq __LP_OFF(C) , RDX;


// Use this macro to read performance counter C into variable V
#define lprof(C, V)				\
     asm volatile (				\
	 "mov $" #C ", %%ecx;"			\
	 __QUOTEME( __LP_ASM(C) )		\
     : "=d" (V)					\
     : "r" (C)					\
	 : "%rax",  "%rcx");

#define lprofd(C, V, CV)			\
     V = CV;					\
     lprof(C, CV)				\
     V = CV - V;

#ifdef __cplusplus
}
#endif

#endif //_LPROF_H
