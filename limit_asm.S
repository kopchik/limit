 #
 # LiMiT Assembly Code
 #
 #   A userland performance counter-based counter profiler.  Allows
 # users with to configure performance counters and read them.  Also
 # provides a variety of userspace utilities to assist in
 # configuration, reading and accounting.
 #
 #    Copyright (C) 2010, John Demme, Columbia University
 #
 #  LiMiT is free software: you can redistribute it and/or modify it
 #  under the terms of the Lesser GNU General Public License as
 #  published by the Free Software Foundation, either version 3 of the
 #  License, or (at your option) any later version.
 #
 #  LiMiT is distributed in the hope that it will be useful, but
 #  WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  Lesser GNU General Public License for more details.
 #
 #  You should have received a copy of the Lesser GNU General Public
 #  License along with LiMiT.  If not, see
 #  <http://www.gnu.org/licenses/>.
 #

# This code defines critical sections.  We use the macros from the
#   header file to make sure they match

#define  __LP_ASM_ONLY
#define RDX %rdx
#define RAX %rax
#define FS %fs
#include <limit.h>

.text
	.global __lp_sec0
	.global __lp_sec1
	.global __lp_sec2
	.global __lp_sec3
	.global __lp_sec_end
	
__lp_sec0:
	__LP_ASM(0)
__lp_sec1:
	__LP_ASM(1)
__lp_sec2:
	__LP_ASM(2)
__lp_sec3:
	__LP_ASM(3)
__lp_sec_end:
	nop