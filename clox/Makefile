all: build

build:
	gcc -O0 -g -Wall -o clox debug.c value.c scanner.c compiler.c memory.c chunk.c table.c vm.c object.c main.c

run: build
	./clox

clean:
	rm -f clox
