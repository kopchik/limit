#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <err.h>
#include "limit.h"

#ifndef ENABLE_LPROF
#define ENABLE_LPROF 0
#endif

#define SAMPLES 1000
#define PERIOD 512
//#define PERIOD 1
#define SIZE 1024

double A[SIZE*SIZE];
double B[SIZE*SIZE];
double volatile C[SIZE*SIZE];

typedef struct Sample {
  uint64_t instr;
  uint64_t cycles;
} Sample;
Sample measurements[SAMPLES];

void *xcalloc(size_t nmemb, size_t size) {
  void *r;
  if ((r=calloc(nmemb, size)) == NULL) {
    err(2, "calloc failed");
  }
  return r;
}

void pollute(void) {
      for (int row=0; row<SIZE; row++) {
        for (int col=0; col<SIZE; col++) {
          double temp = 0.0;
          for (int k=0; k<SIZE; k++) {
            temp += A[row*SIZE+k]*B[k*SIZE+col];
          }
          C[row*SIZE+col] = temp;
        }
      }
}

int main(int argc, char *argv[]) {
#if ENABLE_LPROF
  uint64_t cycles=0, cycles_delta=0, instr=0, instr_delta=0;
  uint64_t cnt=1, cur=0;
#endif

  for (int i=0; i< SIZE*SIZE; i++) {
    A[i] = 1;
    B[i] = 1;
    C[i] = 0;
  }
  fprintf(stderr, "init done\n");


  if (argc==2 && strcmp(argv[1], "polute") == 0) {
    fprintf(stderr, "polluting\n");
    for (int x=0; x<3; x++) {
      pollute();
    }
    return 0;
  }
  else {
    fprintf(stderr, "warm-up\n");
    pollute();
    fprintf(stderr, "profiling\n");
    pid_t pids[10];
    int numpids=argc-1;

    /* parse pids */
    for (int i=0; i<numpids; i++) {
      if ((pids[i] = atol(argv[i+1])) == 0) {
        err(2, "atol(%s)", argv[i]);
      }
    }

    /* stop tasks */
    for(int i=0; i<numpids; i++) {
      fprintf(stderr, "sending SIGSTOP to %d\n", pids[i]);
      if (kill(pids[i], SIGSTOP)) {
        err(2, "kill(%d, SIGSTOP)", pids[i]);
      }
    }

#if ENABLE_LPROF
    if (lprof_init(2, EV_INSTR, EV_CYCLES)) {
      err(2, "lprof_init");
    }
    lprofd(0, instr_delta, instr);
    lprofd(1, cycles_delta, cycles);
    atexit(lprof_close);
#endif

    for (int row=0; row<SIZE; row++) {
      for (int col=0; col<SIZE; col++) {
        double temp = 0.0;
        for (int k=0; k<SIZE; k++) {
          temp += A[row*SIZE+k]*B[k*SIZE+col];
        }
#if ENABLE_LPROF
          cnt++;
          if (cur>=SAMPLES) {
            goto END; }
          if (cur<SAMPLES && cnt % PERIOD == 0) {
            lprofd(0, instr_delta, instr);
            lprofd(1, cycles_delta, cycles);
            measurements[cur].instr  = instr_delta;
            measurements[cur].cycles = cycles_delta;
            cur++;
          }
#endif
        
        C[row*SIZE+col] = temp;
      }
    }

END:
    
    /* send CONT to stopped tasks */
    for(int i=0; i<numpids; i++) {
      fprintf(stderr, "sending SIGCONT to %d\n", pids[i]);
      if (kill(pids[i], SIGSTOP)) {
        err(2, "kill(%d, SIGSTOP)", pids[i]);
      }
    }

    /* output the results */
#if ENABLE_LPROF
    Sample *sample=measurements;
    for (uint64_t i=0; i<cur; i++, sample++) {
      //printf("%lu,%lu\n", sample->cycles, sample->instr);
      printf("%lf,%lu\n", ((double)sample->instr)/sample->cycles, sample->cycles);
    }
#endif

  }

  //free(A);
  //free(B);
  //free(C);
}
