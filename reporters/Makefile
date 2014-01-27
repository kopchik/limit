INC	= -I/lib/modules/`uname -r`/build/include -I../include -I../instr
CC	= gcc -g -fPIC $(INC)

REPORTERS   = liblp_text.so liblp_nothing.so

all: $(REPORTERS)
	cp $(REPORTERS) ../instr


%.o: %.c
	$(CC) -c -o $@ $< 

liblp_text.so : lp_text.o
	$(CC) -shared -o $@ $^

liblp_nothing.so : lp_nothing.o
	$(CC) -shared -o $@ $^


clean: 
	rm -f core *.o *.O *.i *.s *.ii *.class *.a *.so 

depend:
	g++ -E -MM -I$(INCDIR) *.c > DEPENDS
