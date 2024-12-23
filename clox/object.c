#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "table.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) (type *)allocate_object(sizeof(type), object_type);

#ifdef STRINGS_FLEXIBLE_ARRAY_MEMBER

static Obj *allocate_object(size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;
    
    object->next = vm.objects;
    vm.objects = object;
    
    return object;
}

/*
Implement "FNV-1a" hash algorithm.
*/
static u32 hash_string(const char *key, int length)
{
    u32 hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (u8)key[i];
        hash *= 16777619;
    }

    return hash;
}


ObjString *copy_string(const char *chars, int length)
{
    u32 hash = hash_string(chars, length);
    ObjString *interned = table_find_string(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        return interned;
    }

    int total_size = sizeof(ObjString) + length;
    ObjString *string = (ObjString *)allocate_object(total_size + 1, OBJ_STRING);
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    string->length = length;
    string->hash = hash;

    table_set(&vm.strings, string, NIL_VAL);

    return string;
}

ObjString *concatenate_object_strings(ObjString *a, ObjString *b)
{
    int length = a->length + b->length;
    int total_size = sizeof(ObjString) + length;
    ObjString *string = (ObjString *)allocate_object(total_size + 1, OBJ_STRING);
    memcpy(string->chars, a->chars, a->length);
    memcpy(string->chars + a->length, b->chars, b->length);
    string->chars[a->length + b->length] = '\0';
    string->length = length;
    string->hash = hash_string(string->chars, length);
    
    ObjString *interned = table_find_string(&vm.strings, string->chars, length, string->hash);
    if (interned != NULL) {
        FREE(ObjString, string);
        return interned;
    }

    table_set(&vm.strings, string, NIL_VAL);

    return string;
}

void print_object(Value value)
{
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}

#else

static Obj *allocate_object(size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;
    
    object->next = vm.objects;
    vm.objects = object;
    
    return object;
}

static ObjString *allocate_string(const char *chars, int length)
{
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;

    return string;
}

ObjString *take_string(char *chars, int length)
{
    return allocate_string(chars, length);
}

ObjString *copy_string(const char *chars, int length)
{
    char *heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length);
}

void print_object(Value value)
{
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}

#endif