#include "chunk.h"
#include "memory.h"

void init_chunk(Chunk *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

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

    if (line == chunk->lines[chunk->line_index + 1]) {
        chunk->lines[chunk->line_index]++;
    } else {
        chunk->lines[chunk->line_index] = 1;
        chunk->lines[chunk->line_index + 1] = line;
    }
}

int add_constant(Chunk *chunk, Value value)
{
    write_value_array(&chunk->constants, value);
    return chunk->constants.count - 1;
}

u16 get_line(Chunk *chunk, int offset)
{
    for (int i = 0; i < sizeof(chunk->lines); i += 2) {
        u16 count = chunk->lines[i];
        u16 line = chunk->lines[i + 1];

        if (offset == 0) {
            return line;
        }

        while (count > 0 && offset > 0) {
            count--;
            offset--;
        }
    }

    return -1;
}
