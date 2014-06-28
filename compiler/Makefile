LIBS_DIR=../libs

solc: main.c lexer.o parser.o analyser.o generator.o $(LIBS_DIR)/hashmap.o $(LIBS_DIR)/stacklist.o tree_print.o
	$(CC) -o $@ -Wall $^
	cp $@ ../virtual\ machine/$@

lexer.o: lexer.c parser.h def.h
	$(CC) -g -c -Wall $<

parser.o: parser.c def.h
	$(CC) -g -c -Wall $<

analyser.o: analyser.c analyser.h
	$(CC) -g -c -Wall $<

generator.o: generator.c generator.h
	$(CC) -g -c -Wall $<

tree_print.o: tree_print.c tree_print.h def.h
	$(CC) -g -c -Wall $<

hashmap.o: $(LIBS_DIR)/hashmap.c $(LIBS_DIR)/hashmap.h
	$(CC) -g -c -Wall $<

stacklist.o: $(LIBS_DIR)/stacklist.c $(LIBS_DIR)/stacklist.h
	$(CC) -g -c -Wall $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

lexer: main.c lexer.o $(LIBS_DIR)/hashmap.o
	$(CC) -g -o $@ $^ -Wall -DLEXER

parser: main.c tree_print.o lexer.o parser.o $(LIBS_DIR)/hashmap.o
	$(CC) -g -o $@ $^ -Wall -DPARSER

analyser: main.c lexer.o parser.o analyser.o $(LIBS_DIR)/hashmap.o $(LIBS_DIR)/stacklist.o tree_print.o
	$(CC) -g -o $@ $^ -Wall -DANALYSER

generator: main.c lexer.o parser.o analyser.o generator.o $(LIBS_DIR)/hashmap.o $(LIBS_DIR)/stacklist.o tree_print.o
	$(CC) -g -o $@ -Wall $^

.PHONY : clean
clean:
	rm -f \
	  lexer lexer.c \
	  parser parser.c parser.h parser.output *.o \
	  $(LIBS_DIR)/*.o \
	  analyser generator solc
	rm -rf *.dSYM
