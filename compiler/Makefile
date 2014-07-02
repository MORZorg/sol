LIBS_DIR=../libs

solc: main.c lexer.o parser.o analyser.o generator.o $(LIBS_DIR)/hashmap.o $(LIBS_DIR)/stacklist.o tree_print.o
	$(CC) -Os -o $@ -Wall $^
	cp $@ ../virtual\ machine/$@

lexer.o: lexer.c parser.h def.h
	$(CC) -Os -g -c -Wall $<

parser.o: parser.c def.h
	$(CC) -Os -g -c -Wall $<

analyser.o: analyser.c analyser.h
	$(CC) -Os -g -c -Wall $<

generator.o: generator.c generator.h
	$(CC) -Os -g -c -Wall $<

tree_print.o: tree_print.c tree_print.h def.h
	$(CC) -Os -g -c -Wall $<

$(LIBS_DIR)/hashmap.o: $(LIBS_DIR)/hashmap.c $(LIBS_DIR)/hashmap.h
	$(CC) -Os -g -c -Wall -o $@ $<

$(LIBS_DIR)/stacklist.o: $(LIBS_DIR)/stacklist.c $(LIBS_DIR)/stacklist.h
	$(CC) -Os -g -c -Wall -o $@ $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

lexer: main.c lexer.o $(LIBS_DIR)/hashmap.o
	$(CC) -Os -g -o $@ $^ -Wall -DLEXER

parser: main.c tree_print.o lexer.o parser.o $(LIBS_DIR)/hashmap.o
	$(CC) -g -o $@ $^ -Wall -DPARSER

analyser: main.c lexer.o parser.o analyser.o $(LIBS_DIR)/hashmap.o $(LIBS_DIR)/stacklist.o tree_print.o
	$(CC) -g -o $@ $^ -Wall -DANALYSER

generator: main.c lexer.o parser.o analyser.o generator.o $(LIBS_DIR)/hashmap.o $(LIBS_DIR)/stacklist.o tree_print.o
	$(CC) -g -o $@ -Wall $^

.PHONY: clean
clean:
	rm -f \
	  lexer lexer.c \
	  parser parser.c parser.h parser.output *.o \
	  $(LIBS_DIR)/*.o \
	  analyser generator solc \
	  ../virtual\ machine/solc
	rm -rf *.dSYM
