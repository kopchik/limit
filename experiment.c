#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <err.h>
#include "limit.h"

#define SAMPLES 1000
#define PERIOD 172000
#define SIZE 1024

#define ENABLE_LPROF 1
void *xcalloc(size_t nmemb, size_t size) {
  void *r;
  if ((r=calloc(nmemb, size)) == NULL) {
    err(2, "calloc failed");
  }
  return r;
}

int main(int argc, char *argv[]) {
  uint64_t ins=0,ins_delta=0, cycles=0, cycles_delta=0;
#if ENABLE_LPROF
  if (lprof_init(2, EV_INSTR, EV_CYCLES)) {
    err(2, "lprof_init");
  }
#endif

  double *A = xcalloc(SIZE*SIZE, sizeof(double));
  double *B = xcalloc(SIZE*SIZE, sizeof(double));
  double volatile *C = xcalloc(SIZE*SIZE, sizeof(double));

  for (int i=0; i< SIZE*SIZE; i++) {
    A[i] = 1;
    B[i] = 1;
    //C[i] = 0;
  }
  fprintf(stderr, "init done\n");


  if (argc==2 && strcmp(argv[1], "polute") == 0) {
    while (1) {
      fprintf(stderr, "polluting\n");
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
  }
  else {
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
    lprofd(0, ins_delta, ins);
    lprofd(1, cycles_delta, cycles);
#endif
    uint64_t cnt=1, cur=0;
    uint64_t measurements[SAMPLES];
    double ipc;

    for (int row=0; row<SIZE; row++) {
      for (int col=0; col<SIZE; col++) {
        double temp = 0.0;
        for (int k=0; k<SIZE; k++) {
          temp += A[row*SIZE+k]*B[k*SIZE+col];
#if ENABLE_LPROF
          if (cnt++ % PERIOD == 0 && cur<SAMPLES) {
            //lprofd(0, ins_delta, ins);
            lprofd(1, cycles_delta, cycles);
            //ipc = (double)ins_delta/cycles_delta;
            //printf("ins %lu\n", ins_delta);
            measurements[cur++] = cycles_delta;
          }
#endif
        }
        C[row*SIZE+col] = temp;
      }
    }
    
    /* send CONT to stopped tasks */
    for(int i=0; i<numpids; i++) {
      fprintf(stderr, "sending SIGCONT to %d\n", pids[i]);
      if (kill(pids[i], SIGSTOP)) {
        err(2, "kill(%d, SIGSTOP)", pids[i]);
      }
    }

    /* output the results */
#if ENABLE_LPROF
    for (uint64_t i=0; i<cur; i++) {
      //printf("%lf\n", measurements[i]);
      printf("%lu\n", measurements[i]);
    }
#endif

  }
  free(A);
  free(B);
  //free(C);
}
