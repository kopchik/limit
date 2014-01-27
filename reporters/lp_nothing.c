#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <dlfcn.h>
#include <stddef.h>

#include <common.h>
#include <linux/lprof.h>
#include <limit.h>

int32_t exitlprof(int32_t* numFunctions, Result* results){
    /* int32_t i; */
    /* int rc; */

    /* //Shut down LProf */
    /* rc = sys_lprof_config(LPROF_STOP, 1, 0, 0); */
    /* if (rc < 0) { */
    /* 	perror("closing counter"); */
    /* 	exit(-1); */
    /* } */
    
    return 0;
}

#define PERFMON_EVENTSEL_ENABLE (1 << 22)
#define PERFMON_EVENTSEL_INT    (1 << 20)
#define PERFMON_EVENTSEL_USR    (1 << 16)
#define PERFMON_EVENTSEL_OS     (1 << 17)
#define PERFMON_EVENTSEL_PC     (1 << 19)

int32_t initlprof(struct lprof_stats* stats, InstrumentationInfo* ii,
		  int32_t* numFunctions, Result* results) {
    /* int rc, i; */
    /* unsigned long long conf; */

    /* for (i=0; i<MAX_CS; i++) { */
    /* 	if (ii->cs_marker_pairs[2*i] != NULL && */
    /* 	    ii->cs_marker_pairs[2*i+1] != NULL) { */
    /* 	    uint32_t cs_len =  (uint64_t)ii->cs_marker_pairs[2*i+1]  */
    /* 		- (uint64_t)ii->cs_marker_pairs[2*i]; */
    /* 	    rc = sys_lprof_config(LPROF_DFN_CS, i, cs_len,  */
    /* 				  (void*)ii->cs_marker_pairs[2*i]); */
    /* 	    if (rc < 0) { */
    /* 		perror("defining critical section"); */
    /* 		exit(rc); */
    /* 	    }  */

    /* 	    // Debug output (disabled) */
    /*         /\* else { *\/ */
    /* 	    /\* 	unsigned char* csb = (unsigned char*)ii->cs_marker_pairs[2*i]; *\/ */
    /* 	    /\* 	int j; *\/ */
    /* 	    /\* 	printf("Defined critical section %d(%p - %p)\n", i,  *\/ */
    /* 	    /\* 	       ii->cs_marker_pairs[2*i], ii->cs_marker_pairs[2*i+1]); *\/ */
    /* 	    /\* 	printf("\t"); *\/ */
    /* 	    /\* 	for (j=0; j<cs_len; j++) { *\/ */
    /* 	    /\* 	    printf("%02x ", csb[j]); *\/ */
    /* 	    /\* 	} *\/ */
    /* 	    /\* 	printf("\n"); *\/ */
    /* 	    /\* } *\/ */
    /* 	} */
    /* } */

    /* conf = 0xC0; //Instructions Retired */
    /* //conf = 0x3C; //UnHalted Core Cycles */

    /* conf = PERFMON_EVENTSEL_USR | PERFMON_EVENTSEL_ENABLE | */
    /* 	(conf & ~PERFMON_EVENTSEL_OS & ~PERFMON_EVENTSEL_PC & */
    /* 	 (0xFFF | (0xFF << 24))); */

    /* rc = sys_lprof_config(LPROF_START, 1, conf, stats); */
    /* if (rc < 0) { */
    /* 	perror("opening counter"); */
    /* 	exit(rc); */
    /* } */

    return 0;
}

