solm: main.c
	$(CC) -o $@ $^

machine: main.c
	$(CC) -g -o $@ $^

lexer: machine.c lexer.o
	$(CC) -g -o $@ $^

lexer.o: lexer.c parser.h def.h
	$(CC) -g -c $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser: machine.c lexer.o parser.o
	$(CC) -g -o $@ $^

parser.o: parser.c def.h
	$(CC) -g -c $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

.PHONY : clean
clean:
	rm -f \
	  machine solm
	rm -rf *.dSYM

