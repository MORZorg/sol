lexer: lexer.lex def.h
	flex -o lexer.c lexer.lex
	cc -o lexer lexer.c
