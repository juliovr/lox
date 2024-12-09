#include <stdlib.h>
#include <stdio.h>
#include "chunk.h"
#include "memory.h"

void init_chunk(Chunk *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->line_index = -1;

    init_value_array(&chunk->constants);
    // chunk->capacity = 8;
    // chunk->code = GROW_ARRAY(u8, chunk->code, 0, chunk->capacity);
}

void free_chunk(Chunk *chunk)
{
    FREE_ARRAY(u8, chunk->code, chunk->capacity);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

void write_chunk(Chunk *chunk, u8 byte, int line)
{
    if (chunk->capacity < chunk->count + 1) {
        int old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = GROW_ARRAY(u8, chunk->code, old_capacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;

    if (chunk->line_index >= 0 && line == chunk->lines[chunk->line_index + 1]) {
        chunk->lines[chunk->line_index]++;
    } else {
        if (chunk->line_index < 0) {
            chunk->line_index = 0;
        } else {
            chunk->line_index += 2;
        }
        
        chunk->lines[chunk->line_index] = 1;
        chunk->lines[chunk->line_index + 1] = line;
    }
}

#define MAX_U8 (1 << 8)
#define MAX_U16 (1 << 16)

int opcode_size_operands(OpCode opcode)
{
    switch (opcode) {
        case OP_CONSTANT: return 1;
        case OP_CONSTANT_LONG: return 3;
        case OP_RETURN: return 0;
    }

    return 0;
}

void write_constant(Chunk* chunk, Value value, int line)
{
    if ((int)value >= MAX_U16) {
        printf("Constant value %g at line %d is too big\n.", value, line);
        exit(1);
    }

    OpCode opcode = OP_CONSTANT_LONG;
    if ((int)value < MAX_U8) {
        opcode = OP_CONSTANT;
    }

    u8 size_bytes = opcode_size_operands(opcode);

    int constant_index = add_constant(chunk, value, size_bytes);
    write_chunk(chunk, opcode, line);
    write_chunk(chunk, constant_index, line);
}

int add_constant(Chunk *chunk, Value value, u8 size_bytes)
{
    write_value_array(&chunk->constants, value, size_bytes);
    return chunk->constants.count - size_bytes;
}

u16 get_line(Chunk *chunk, int offset)
{
    for (int i = 1; i < sizeof(chunk->lines); i += 2) {
        u16 count = chunk->lines[i - 1];
        u16 line = chunk->lines[i];

        while (count > 0 && offset > 0) {
            count--;
            offset--;
        }

        if (count == 0) {
            continue;
        }

        if (offset == 0) {
            return line;
        }
    }

    return -1;
}
