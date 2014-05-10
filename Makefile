solc: lex.o parser.o tree.o
	$(CC) -g -o $@ $^

lexer.o: lexer.c parser.h def.h
	$(CC) -g -c $<

parser.o: parser.c def.h
	$(CC) -g -c $<

tree.o: tree.c def.h
	$(CC) -g -c $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

lexer: main.c lexer.c
	$(CC) -g -o $@ $^ -DLEXER

parser: main.c lexer.o parser.o
	$(CC) -g -o $@ $^ -DPARSER

.PHONY : clean
clean:
	rm -f lexer lexer.c lexer.o \
	  parser parser.c parser.h parser.o parser.output
	rm -rf lexer.dSYM parser.dSYM
