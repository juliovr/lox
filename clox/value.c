#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"
#include "object.h"

void init_value_array(ValueArray *array)
{
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void write_value_array(ValueArray *array, Value value, u8 size_bytes)
{
    if (array->capacity < array->count + size_bytes) {
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(Value, array->values, old_capacity, array->capacity);
    }

    // TODO: fix the multibyte store. Maybe it is better to treat the values as (u32 *), where the first 8 bits belongs to
    // the instruction and the last 24 are the double value.
    // for (int i = 0; i < size_bytes; i++) {
    //     array->values[array->count + i] = (((int)((int *)&value) >> (i * 8)) & 0xFF);
    // }
    array->values[array->count] = value;
    array->count += size_bytes;
}

void free_value_array(ValueArray *array)
{
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(Value value)
{
    switch (value.type) {
        case VAL_BOOL: 
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NIL: 
            printf("nil");
            break;
        case VAL_NUMBER:
            printf("%g", AS_NUMBER(value));
            break;
        case VAL_OBJ:
            print_object(value);
            break;
        default: return; // Unreachable
    }
}

bool values_equal(Value a, Value b)
{
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:    return AS_OBJ(a) == AS_OBJ(b);
        default:         return false; // Unreachable
    }
}
