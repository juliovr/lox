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
    Value value = NUMBER_VAL((double)(((int)AS_NUMBER(chunk->constants.values[constant_index + 2]) << 16) |
                                      ((int)AS_NUMBER(chunk->constants.values[constant_index + 1]) << 8)  |
                                      ((int)AS_NUMBER(chunk->constants.values[constant_index + 0]) << 0)));
    print_value(value);
    printf("'\n");

    return offset + 2;
}

static int simple_instruction(const char *name, int offset)
{
    printf("%s\n", name);
    
    return offset + 1;
}

static int byte_instruction(const char *name, Chunk *chunk, int offset)
{
    u8 slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    
    return offset + 2;
}

static int jump_instruction(const char *name, int sign, Chunk *chunk, int offset)
{
    u16 jump = (((u16)chunk->code[offset + 1]) << 8) | ((u16)chunk->code[offset + 2]);
    printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign*jump);

    return offset + 3;
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
        case OP_NIL:
            return simple_instruction("OP_NIL", offset);
        case OP_TRUE:
            return simple_instruction("OP_TRUE", offset);
        case OP_FALSE:
            return simple_instruction("OP_FALSE", offset);
        case OP_POP:
            return simple_instruction("OP_POP", offset);
        case OP_GET_LOCAL:
            return byte_instruction("OP_GET_LOCAL", chunk, offset);
        case OP_SET_LOCAL:
            return byte_instruction("OP_SET_LOCAL", chunk, offset);
        case OP_GET_GLOBAL:
            return constant_instruction("OP_GET_GLOBAL", chunk, offset);
        case OP_DEFINE_GLOBAL:
            return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);
        case OP_SET_GLOBAL:
            return constant_instruction("OP_SET_GLOBAL", chunk, offset);
        case OP_EQUAL:
            return simple_instruction("OP_EQUAL", offset);
        case OP_GREATER:
            return simple_instruction("OP_GREATER", offset);
        case OP_LESS:
            return simple_instruction("OP_LESS", offset);
        case OP_ADD: 
            return simple_instruction("OP_ADD", offset);
        case OP_SUBTRACT: 
            return simple_instruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY: 
            return simple_instruction("OP_MULTIPLY", offset);
        case OP_DIVIDE: 
            return simple_instruction("OP_DIVIDE", offset);
        case OP_NOT:
            return simple_instruction("OP_NOT", offset);
        case OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);
        case OP_PRINT:
            return simple_instruction("OP_PRINT", offset);
        case OP_JUMP:
            return jump_instruction("OP_JUMP", 1, chunk, offset);
        case OP_JUMP_IF_FALSE:
            return jump_instruction("OP_JUMP_IF_FALSE", 1, chunk, offset);            
        case OP_LOOP:
            return jump_instruction("OP_LOOP", -1, chunk, offset);
        case OP_SWITCH:
            return simple_instruction("OP_SWITCH", offset);
        case OP_CALL:
            return byte_instruction("OP_CALL", chunk, offset);
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
