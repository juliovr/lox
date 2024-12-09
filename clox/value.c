#include <stdio.h>

#include "memory.h"
#include "value.h"

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

    for (int i = 0; i < size_bytes; i++) {
        array->values[array->count + i] = (((int)value >> (i * 8)) & 0xFF);
    }

    array->count += size_bytes;
}

void free_value_array(ValueArray *array)
{
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(Value value)
{
    printf("%g", value);
}
