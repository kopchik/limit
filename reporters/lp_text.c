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

static FILE* out;
static FILE* outb;

struct lprof_stats* gstats;

int32_t exitlprof(int32_t* numFunctions, Result* results){
    int32_t i;
    int rc;

    /* printf("gstats: last[1]: 0x%lx\n", gstats->last[1]); */
    /* printf("gstats: last[1].off: %lu\n", offsetof(struct lprof_stats, last[1])); */

    //Shut down LProf
    rc = sys_lprof_config(LPROF_STOP, 1, 0, 0);
    if (rc < 0) {
    	perror("closing counter");
    	exit(-1);
    }
    

    fprintf(out, "%s, %s, %s, %s, %s, %s, %s, %s\n",
	    "P", "Calls", "Inc", "Ints", "CS", "Name", "IP", "&r");
    for (i = 0; i < *numFunctions; i++){
	Result r = results[i];
	if (r.name == NULL)
	    r.name = (char*)"Bad ptr";
	if (r.p || r.callCount || r.inc)
	{
	    fprintf(out, "%lu,%lu,%lu,%lu,\"%s\",0x%08lx,%p\n",
		    r.p, r.callCount, r.inc, (uint64_t)r.context_switches, 
		    r.name, r.ip, &results[i]);
	    fflush(out);
	}
    }

    /* fprintf(out, "\nOffsets:\n"); */
    /* fprintf(out, "\t%s: %u\n", "P", (unsigned int)offsetof(Result, p)); */
    /* fprintf(out, "\t%s: %u\n", "callCount", (unsigned int)offsetof(Result, callCount)); */
    /* fprintf(out, "\t%s: %u\n", "inc", (unsigned int)offsetof(Result, inc)); */
    /* fprintf(out, "\t%s: %u\n", "name", (unsigned int)offsetof(Result, name)); */
    /* fprintf(out, "\t%s: %u\n", "ip", (unsigned int)offsetof(Result, ip)); */

    fclose(out);

    return i;
}

#define PERFMON_EVENTSEL_ENABLE (1 << 22)
#define PERFMON_EVENTSEL_INT    (1 << 20)
#define PERFMON_EVENTSEL_USR    (1 << 16)
#define PERFMON_EVENTSEL_OS     (1 << 17)
#define PERFMON_EVENTSEL_PC     (1 << 19)

int32_t initlprof(struct lprof_stats* stats, InstrumentationInfo* ii,
		  int32_t* numFunctions, Result* results) {
    int rc, i;
    unsigned long long conf;
    gstats = stats;

    out = fopen("lp.csv", "w");

    fprintf(out, "LProf Results\n\n");
    fflush(out);

    for (i=0; i<MAX_CS; i++) {
	if (ii->cs_marker_pairs[2*i] != NULL &&
	    ii->cs_marker_pairs[2*i+1] != NULL) {
	    uint32_t cs_len =  (uint64_t)ii->cs_marker_pairs[2*i+1] 
		- (uint64_t)ii->cs_marker_pairs[2*i];
	    rc = sys_lprof_config(LPROF_DFN_CS, i, cs_len, 
				  (void*)ii->cs_marker_pairs[2*i]);
	    //printf("CS len:%d, ptr: %p\n", cs_len, (void*)ii->cs_marker_pairs[2*i]);
	    if (rc < 0) {
		perror("defining critical section");
		exit(rc);
	    } 

	    // Debug output (disabled)
            /* else { */
	    /* 	unsigned char* csb = (unsigned char*)ii->cs_marker_pairs[2*i]; */
	    /* 	int j; */
	    /* 	printf("Defined critical section %d(%p - %p)\n", i,  */
	    /* 	       ii->cs_marker_pairs[2*i], ii->cs_marker_pairs[2*i+1]); */
	    /* 	printf("\t"); */
	    /* 	for (j=0; j<cs_len; j++) { */
	    /* 	    printf("%02x ", csb[j]); */
	    /* 	} */
	    /* 	printf("\n"); */
	    /* } */
	}
    }

    //conf = 0xC0; //Instructions Retired
    conf = 0x3C; //UnHalted Core Cycles

    conf = PERFMON_EVENTSEL_USR | PERFMON_EVENTSEL_ENABLE |
    	(conf & ~PERFMON_EVENTSEL_OS & ~PERFMON_EVENTSEL_PC &
    	 (0xFFF | (0xFF << 24)));

    //printf("Stats: %p\n", stats);
    rc = sys_lprof_config(LPROF_START, 1, conf, stats);
    if (rc < 0) {
    	perror("opening counter");
    	exit(rc);
    }

    return 0;
}

