all: build

build:
	gcc -O0 -g -Wall -o clox debug.c value.c scanner.c compiler.c memory.c chunk.c vm.c main.c

run: build
	./clox