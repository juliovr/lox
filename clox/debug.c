#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassemble_chunk(Chunk *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count; ) {
        offset = disassemble_instruction(chunk, offset);
    }
}

static int constant_instruction(const char *name, Chunk *chunk, int offset)
{
    u8 constant_index = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant_index);
    print_value(chunk->constants.values[constant_index]);
    printf("'\n");

    return offset + 2;
}

static int constant_long_instruction(const char *name, Chunk *chunk, int offset)
{
    u8 constant_index = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant_index);
    Value value = (Value)(((int)chunk->constants.values[constant_index + 2] << 16) |
                          ((int)chunk->constants.values[constant_index + 1] << 8) |
                          ((int)chunk->constants.values[constant_index + 0] << 0));
    print_value(value);
    printf("'\n");

    return offset + 2;
}

static int simple_instruction(const char *name, int offset)
{
    printf("%s\n", name);
    
    return offset + 1;
}

int disassemble_instruction(Chunk *chunk, int offset)
{
    printf("%04d ", offset);

    if (offset > 0 && get_line(chunk, offset) == get_line(chunk, offset - 1)) {
        printf("   | ");
    } else {
        printf("%4d ", get_line(chunk, offset));
    }

    u8 instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return constant_long_instruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
