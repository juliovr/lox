#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

#define LINES_CAPACITY (1 << (sizeof(u16) * 8))

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_CALL,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_SWITCH,
    OP_RETURN,
} OpCode;

int opcode_size_operands(OpCode opcode);

typedef struct {
    int count;
    int capacity;
    u8 *code;
    int line_index;
    u16 lines[LINES_CAPACITY]; // Note: In Run-length encoding: even_index = count, odd_index = line_number
    ValueArray constants;
} Chunk;

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
void write_chunk(Chunk *chunk, u8 byte, int line);
void write_constant(Chunk* chunk, Value value, int line);
int add_constant(Chunk *chunk, Value value, u8 size_bytes);
u16 get_line(Chunk *chunk, int offset);
OpCode get_constant_opcode(Value value);

#endif
