VER=1.3
NAME=liblimit-$(VER)
CFLAGS=-I/lib/modules/`uname -r`/build/include -I. -L. -g -O0 -save-temps -m64
LDFLAGS=-m64 -L.
ASFLAGS=-m64 -I.

all: liblimit.a hello

hello: liblimit.a hello.c
	$(CC) $(CFLAGS) -o hello hello.c -llimit -ldl

liblimit.a: limit.o limit_asm.o
	ar -crs $@ $^

limit.o: limit.h
limit_asm.o: limit.h

install:
	cp liblimit.a /usr/local/lib
	cp -r include/* /usr/local/include

clean:
	rm -f *.o *.i *.a *.s hello
