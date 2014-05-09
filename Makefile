solc: lex.o parser.o tree.o
	cc -g -o solc lexer.o parser.o tree.o

lexer.o: lexer.c parser.h def.h
	cc -g -c lexer.c 

parser.o: parser.c def.h
	cc -g -c parser.c

tree.o: tree.c def.h
	cc -g -c tree.c

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o lexer.c lexer.lex

parser.h: parser.y def.h
	bison -vd -o parser.c parser.y

lexer: lexer.c
	cc -g -o lexer lexer.c

parser: parser.o lexer.o
	cc -g -o parser parser.o lexer.o

