solvm: main.c
	$(CC) -o $@ $^

machine: main.c executor.o support_structures.o parser.o lexer.o dataIO.o
	$(CC) `python3-config --cflags --ldflags` -g -o $@ $^ -DMACHINE

executor.o: executor.c executor.h support_structures.h dataIO.h
	$(CC) `python3-config --cflags` -g -c $<

support_structures.o: support_structures.c support_structures.h def.h
	$(CC) -g -c $<

lexer: main.c lexer.o
	$(CC) -g -o $@ $^

lexer.o: lexer.c parser.h def.h
	$(CC) -g -c $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser: main.c lexer.o parser.o tree_print.o def.h
	$(CC) -g -o $@ $^ -DPARSER

parser.o: parser.c def.h 
	$(CC) -g -c $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

dataIO.o: dataIO.c dataIO.h
	$(CC) `python3-config --cflags` -g -c $<

.PHONY : clean
clean:
	rm -f \
	  machine solm lexer parser *.o
	rm -rf *.dSYM

