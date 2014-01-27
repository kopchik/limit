VER=1.3
NAME=liblimit-$(VER)

all: bin src/liblimit.a hello # instr/mutator reporters/liblp_text.so
	#cp src/liblimit.a instr/mutator reporters/liblp_text.so bin/
	#mv bin/mutator bin/limit
	#rm -f bin/liblprof.so
	#ln -s bin/liblp_text.so bin/liblprof.so

hello: hello.c
	gcc -O3 -o hello hello.c -llimit -ldl
bin:
	mkdir bin

src/liblimit.a:
	cd src; make

instr/mutator: src/liblimit.a
	cd instr; make

reporters/liblp_text.so:
	cd reporters; make

install:
	cp bin/liblimit.a /usr/local/lib
	cp -r include/* /usr/local/include

clean:
	cd reporters; make clean
	cd instr; make clean
	cd src;   make clean
