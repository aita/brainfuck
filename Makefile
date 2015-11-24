.s.o:
	$(CC) -c $@ $<

brainfuck: main.o vm.o x86.o
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f brainfuck *.o
