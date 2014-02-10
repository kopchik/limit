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
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <linux/lprof.h>
#include <limit.h>
#include <assert.h>

//From limit_asm.S -- Critical Sections
extern void __lp_sec0(void);
extern void __lp_sec1(void);
extern void __lp_sec2(void);
extern void __lp_sec3(void);
extern void __lp_sec_end(void);

__thread struct lprof_stats __lp_stats;
static int num_ctrs;
static uint32_t confs[MAX_COUNTERS];

// Register a crictical section with the kernel
static void registerCS(int i, void* b, void* e) {
    int rc;
    size_t cs_len = (uint64_t)e - (uint64_t)b;
    rc = sys_lprof_config(LPROF_DFN_CS, i, cs_len, 
        (void*)b);
    if (rc < 0)
  perror("Error registering CS");
}

// Initialize counters
int lprof_init(unsigned int num, ...) {
    va_list args;
    int rc, ctr = 1;
    unsigned int conf;
    __lp_stats.rdpmc_inprogress = 0;
    memset(&__lp_stats, 0, sizeof(struct lprof_stats));

    //Register critical sections for first 4 counters
    registerCS(0, __lp_sec0, __lp_sec1);
    registerCS(1, __lp_sec1, __lp_sec2);
    registerCS(2, __lp_sec2, __lp_sec3);
    registerCS(3, __lp_sec3, __lp_sec_end);

    //Configure each counter, staring with 1
    va_start(args, num);
    for (ctr=0; ctr<num; ctr++) {
  size_t count = 0;
  conf = va_arg(args, unsigned int) & 0xFFFF;;
  fprintf(stderr, "Configuring counter %d as %04x\n", ctr, conf);
  do {
      conf |= PERFMON_EVENTSEL_USR;
      confs[ctr] = conf;
      rc = sys_lprof_config(LPROF_START, ctr, conf, &__lp_stats);
      count++;
  } while (rc < 0 && errno == EBUSY && count < 5);

  if (rc < 0) {
      perror("Error configuring counter");
  }
    }
    va_end(args);

    num_ctrs = num;

    return rc;
}

void lprof_debug(void) {
  if(sys_lprof_config(LPROF_DBG, 0, 0, 0)) {
    perror("lprof_debug");
  }
}

//Close each counter
void lprof_close() {
    int i;
    for (i=0; i<num_ctrs; i++)
  sys_lprof_config(LPROF_STOP, i, 0, 0);
    fprintf(stderr, "counters closed\n");
}

struct notif_list {
    volatile struct notif_list* next;
    thread_notif notif;
    void* data;
};

static volatile struct notif_list *lprof_notif_list = NULL;

void lprof_thread_notify(thread_notif notif, void* data) {
    struct notif_list * ni = (struct notif_list*)malloc(sizeof(struct notif_list));
    ni->data = data;
    ni->notif = notif;
    
    do {
  ni->next = lprof_notif_list;
    } while (!__sync_bool_compare_and_swap(&lprof_notif_list, ni->next, ni));
}

#include <dlfcn.h>
#include <pthread.h>

static int (*real_pthread_create)(pthread_t*, 
          const pthread_attr_t *,
          void *(*start_routine) (void *),
          void*) = NULL;

typedef struct {
    void* (*user_routine)(void*);
    void* user_args;
} user_thread;

static __thread user_thread* ut;
void* limit_thread(void* arg) {
    volatile struct notif_list *nl;
    size_t i;
    ut = (user_thread*)arg;
    for (i=1; i<=num_ctrs; i++) {
  int rc = sys_lprof_config(LPROF_START, i, confs[i], &__lp_stats);
  if (rc != 0) {
      perror("Attempting to start counter on new thread");
  }
    }

    // Notify that new thread is in town
    nl = lprof_notif_list;
    while (nl) {
  nl->notif(0, nl->data);
  nl = nl->next;
    }

    printf("Limit_thread start\n");
    ut->user_routine(ut->user_args);
    printf("Limit_thread end\n");

    // Notify that new thread is leaving
    nl = lprof_notif_list;
    while (nl) {
  nl->notif(1, nl->data);
  nl = nl->next;
    }
    
    free(ut);
    lprof_close();
}

void pthread_exit(void* ptr)
{
    volatile struct notif_list *nl;
    void (*real_pthread_exit)(void*) = dlsym(RTLD_NEXT, "pthread_exit");

    printf("Exit intercept!\n");

    // Notify that new thread is leaving
    nl = lprof_notif_list;
    while (nl) {
  nl->notif(1, nl->data);
  nl = nl->next;
    }
    
    free(ut);
    lprof_close();
    real_pthread_exit(ptr);
}

int pthread_create(pthread_t *newthread,
       const pthread_attr_t *attr,
       void *(*start_routine) (void *),
       void *arg) 
{
    user_thread* ut = NULL;
    if (!real_pthread_create) {
  real_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
    }

    ut = malloc(sizeof(user_thread));
    assert(ut && "Could not allocate memory.");
    ut->user_routine = start_routine;
    ut->user_args = arg;
    return real_pthread_create(newthread, attr, limit_thread, ut);
}
