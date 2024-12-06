#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

#define LINES_CAPACITY (1 << (sizeof(u16) * 8))

typedef enum {
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    u8 *code;
    u16 line_index;
    u16 lines[LINES_CAPACITY]; // Note: In Run-length encoding: even_index = count, odd_index = line_number
    ValueArray constants;
} Chunk;

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
void write_chunk(Chunk *chunk, u8 byte, int line);
int add_constant(Chunk *chunk, Value value);
u16 get_line(Chunk *chunk, int offset);

#endif
