all: build

build:
	gcc -O0 -g -o clox debug.c value.c memory.c chunk.c main.c

run: build
	./clox