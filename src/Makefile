CFLAGS=-I/lib/modules/`uname -r`/build/include 	-I../include	\
	-g -O0 -save-temps -m64
CXXFLAGS=$(CFLAGS)
LDFLAGS=-m64
ASFLAGS=-m64 -I../include

all: liblimit.a
	cp $^ ../bin

liblimit.a: limit.o limit_asm.o limit-cpp.o
	ar -crs $@ $^

clean:
	rm -f *.o *.i *.a *.s

limit.o: ../include/limit.h
limit_asm.o: ../include/limit.h