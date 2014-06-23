solm: main.c
	$(CC) -o $@ $^

machine: main.c
	$(CC) -g -o $@ $^

.PHONY : clean
clean:
	rm -f \
	  machine solm
	rm -rf *.dSYM

