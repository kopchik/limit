VER=1.4
NAME=liblimit-$(VER)
CFLAGS= -O3 -std=gnu99
CC=clang

LPROF ?= 1
ifeq ($(LPROF), 1)
  LDFLAGS +=  -L.
  ASFLAGS += -I.
  CFLAGS  += -I/lib/modules/`uname -r`/build/include -I. -L. -llimit -ldl
  CFLAGS  += -DENABLE_LPROF=1
  all: liblimit.a experiment
else
  all: experiment
endif


limit.o: limit.h
limit_asm.o: limit.h
liblimit.a: limit.o limit_asm.o
	ar -crs $@ $^

experiment: experiment.c
	$(CC) $(CFLAGS) -o experiment experiment.c

stabtest: experiment
	{ for x in `seq 10`; do time taskset -c 0 ./experiment; done } 2>&1 | grep real


install:
	cp liblimit.a /usr/local/lib
	cp limit.h /usr/local/include

clean:
	rm -f *.o *.i *.a *.s hello experiment
