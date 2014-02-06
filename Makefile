VER=1.3
NAME=liblimit-$(VER)
CFLAGS=-I/lib/modules/`uname -r`/build/include -I. -L. -m64
LDFLAGS=-m64 -L.
ASFLAGS=-m64 -I.

all: liblimit.a hello experiment

limit.o: limit.h
limit_asm.o: limit.h
liblimit.a: limit.o limit_asm.o
	ar -crs $@ $^

hello: liblimit.a hello.c
	$(CC) $(CFLAGS) -O0 -save-temps -o hello hello.c -llimit -ldl

experiment: experiment.c
	$(CC) $(CFLAGS) -std=gnu99 -O3 -o experiment experiment.c -llimit -ldl


install:
	cp liblimit.a /usr/local/lib
	cp limit.h /usr/local/include

clean:
	rm -f *.o *.i *.a *.s hello
