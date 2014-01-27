LiMiT -- LightweIght MonItoring Toolkit
  http://castl.cs.columbia.edu/limit

Thanks for downloading LiMiT.  LiMiT allows fast (5 instruction)
access to performance counters on modern x64 processors.


== Installation == 

LiMiT installation is relatively complicated (requiring a kernel patch
_and_ userland API) so it is only recommended for Linux gurus.

It is necessary to install LiMiT's kernel patch (that's right, patch
not module) in order for the userland API to operate properly.  This
patch is included for the Linux 2.6.32 kernel in the src/ directory.
Please refer to your distribution's documentation for details on
applying patches.  Pre-built kernels with LiMiT appled are available
for Ubuntu 10.04 on the LiMiT website.

Once the kernel patch is appled, use "make" to build the userland
library.  Binaries will be placed into bin/ and include files in
include. You may place them wherever you like on your system; "sudo
make install" will put them in "/usr/local".

== Usage & Documentation ==

LiMiT is a relatively new project so documentation is scarce.  There
are two interfaces, the C library and the automatic instrumentor.

The automatic instrumentor uses PEBIL to statically rewrite binaries
to include function granularity instrumentation.  All code and details
are in instr/ and reporters/.  It doesn't work on all programs and
will sometimes change program semantics.

The C API is very useful for those building instrumentation
themselves.  It may be inserted into existing code or built into
shared libraries for function interposition.  This latter option
sometimes requires modification to the LiMiT library to deal with
linking issues.  Contact LiMiT's authors if you need help.

An example of the normal C API is found below:

// Compile with: gcc -o example example.c -llimit
#include <limit.h>
#include <stdint.h>

double mpr[30];
uint64_t switches[30];
uint64_t cycles[30];

int main(void) {
  uint64_t br_last, brm_last;
  uint64_t c, br, brm;

  lprof_init(3, EV_CYCLES, EV_BRANCH, EV_BRANCH_MISS);

  for (i=0; i<30; i++) {
    __lp_stats.context_switches = 0; // Optional: reset kernel stats

    lprof(2, br_last);  //Optional: 
    lprof(3, brm_last); // sample just before call

    function_to_watch(); // Do something

    lprof(1, c);        //Get cumulative cycles
    lprofd(2, br,  br_last);    //Get delta branches
    lprofd(3, brm, brm_last);   //Get delta mispreds

    cycles[i] = c; //Store cycle count
    mpr[i] = (double)brm/(double)br; //Calc mispred rate
    switches[i] = __lp_stats.context_switches; // Store kernel stats
  }

  lprof_close();
}



== License & Copyright ==

LiMiT is Copyright 2010, John Demme, Columbia University
 
LiMiT is free software: you can redistribute it and/or modify it under
the terms of the Lesser GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.
 
LiMiT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the Lesser GNU General Public
License for more details.

You should have received a copy of the Lesser GNU General Public
License along with LiMiT.  If not, see <http://www.gnu.org/licenses/>.

This distribution also includes compatible, modified copies of PEBIL
and udis86 which are GPL licensed.  See instr/pebil/ for details.