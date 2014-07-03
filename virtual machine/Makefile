LIBS_DIR=../libs

solvm: main.c executor.o support_structures.o parser.o lexer.o dataIO.o tree_print.o $(LIBS_DIR)/hashmap.o
	$(CC) -Os -o $@ $^ `python3-config --cflags --ldflags`

machine: main.c executor.o support_structures.o parser.o lexer.o dataIO.o tree_print.o $(LIBS_DIR)/hashmap.o
	$(CC) -Os -g -o $@ $^ -DMACHINE `python3-config --cflags --ldflags`

executor.o: executor.c executor.h support_structures.h dataIO.h tree_print.h
	$(CC) -Os `python3-config --cflags` -g -c $< -DDEBUG

support_structures.o: support_structures.c support_structures.h def.h $(LIBS_DIR)/hashmap.h
	$(CC) -Os -g -c $< -DDEBUG

tree_print.o: tree_print.c tree_print.h def.h
	$(CC) -Os -g -c $<

lexer: main.c lexer.o
	$(CC) -g -o $@ $^

lexer.o: lexer.c parser.h def.h
	$(CC) -Os -g -c $<

lexer.c: lexer.lex parser.y parser.h parser.c def.h
	flex -o $@ $<

parser: main.c lexer.o parser.o tree_print.o def.h
	$(CC) -g -o $@ $^ -DPARSER

parser.o: parser.c def.h 
	$(CC) -Os -g -c $<

parser.h: parser.y def.h
	bison -vd -o parser.c $<

dataIO.o: dataIO.c dataIO.h
	$(CC) -Os `python3-config --cflags` -g -c $<

$(LIBS_DIR)/hashmap.o: $(LIBS_DIR)/hashmap.c $(LIBS_DIR)/hashmap.h
	$(CC) -Os -g -c -Wall -o $@ $<

.PHONY: clean
clean:
	rm -f \
	  machine solvm lexer parser *.o \
	  $(LIBS_DIR)/*.o
	rm -rf *.dSYM

