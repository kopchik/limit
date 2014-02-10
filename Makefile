VER=1.4
NAME=liblimit-$(VER)
CFLAGS=-I/lib/modules/`uname -r`/build/include -I. -L. -m64 -O3 -std=gnu99
LDFLAGS=-m64 -L.
ASFLAGS=-m64 -I.
CC=clang

all: liblimit.a hello experiment

limit.o: limit.h
limit_asm.o: limit.h
liblimit.a: limit.o limit_asm.o
	ar -crs $@ $^

hello: liblimit.a hello.c
	$(CC) $(CFLAGS) -o hello hello.c -llimit -ldl

experiment: experiment.c
	$(CC) $(CFLAGS) -o experiment experiment.c -llimit -ldl


install:
	cp liblimit.a /usr/local/lib
	cp limit.h /usr/local/include

clean:
	rm -f *.o *.i *.a *.s hello experiment
