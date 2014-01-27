VER=1.3
NAME=liblimit-$(VER)

all: bin src/liblimit.a instr/mutator reporters/liblp_text.so
	cp src/liblimit.a instr/mutator reporters/liblp_text.so bin/
	mv bin/mutator bin/limit
	rm -f bin/liblprof.so
	ln -s bin/liblp_text.so bin/liblprof.so

bin:
	mkdir bin

src/liblimit.a:
	cd src; make

instr/mutator: src/liblimit.a
	cd instr; make

reporters/liblp_text.so:
	cd reporters; make

deb: all
	mkdir -p debian/DEBIAN
	cp control debian/DEBIAN
	mkdir -p debian/usr/bin
	mkdir -p debian/usr/lib
	mkdir -p debian/usr/include
	cp bin/liblimit.a debian/usr/lib
	cp bin/liblp_*.so    debian/usr/lib
	cp include/*.h debian/usr/include
	cp bin/limit debian/usr/bin
	fakeroot dpkg-deb --build debian
	mv debian.deb liblimit-dev_$(VER)_amd64.deb

install:
	cp bin/liblimit.a /usr/local/lib
	cp -r include/* /usr/local/include

clean:
	rm -f *.deb *.tar.gz
	cd reporters; make clean
	cd instr; make clean
	cd src;   make clean

FILES=README COPYING control Makefile src include instr reporters
tarball:
	make clean
	rm -rf limit-$(VER)
	mkdir limit-$(VER)
	for f in $(FILES); do svn export $${f} limit-$(VER)/$${f}; done;
	tar -czf limit-$(VER).tar.gz limit-$(VER)