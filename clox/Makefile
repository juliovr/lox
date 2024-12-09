all: build

build:
	gcc -O0 -g -Wall -o clox debug.c value.c memory.c chunk.c vm.c main.c

run: build
	./clox