solc: lex.o parser.o analyser.o
	$(CC) -g -o $@ $^

lexer.o: lexer.c parser.h def.h
	$(CC) -g -c $< -DDEBUG

parser.o: parser.c def.h
	$(CC) -g -c $<

analyser.o: analyser.c analyser.h
	$(CC) -g -c $<

tree_print.o: tree_print.c tree_print.h def.h
	$(CC) -g -c $<

hashmap.o: hashmap.c hashmap.h
	$(CC) -g -c $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

lexer: main.c lexer.o hashmap.o
	$(CC) -g -o $@ $^ -DLEXER

parser: main.c tree_print.o lexer.o parser.o hashmap.o
	$(CC) -g -o $@ $^ -DPARSER

analyser: main.c lexer.o parser.o analyser.o hashmap.o stacklist.o tree_print.o
	$(CC) -g -o $@ $^ -DANALYZER

.PHONY : clean
clean:
	rm -f \
	  lexer lexer.c \
	  parser parser.c parser.h parser.output *.o \
	  analyser
	rm -rf *.dSYM
