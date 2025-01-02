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

ObjClosure *new_closure(ObjFunction *function)
{
    ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalue_count);
    for (int i = 0; i < function->upvalue_count; i++) {
        upvalues[i] = NULL;
    }

    ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;

    return closure;
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

static void print_function(ObjFunction *function)
{
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    
    printf("<fn %s>", function->name->chars);
}

ObjFunction *new_function()
{
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION)
    function->arity = 0;
    function->upvalue_count = 0;
    function->name = NULL;
    init_chunk(&function->chunk);

    return function;
}

ObjNative *new_native(NativeFn function, int arity)
{
    ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->arity = arity;
    native->function = function;

    return native;
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

ObjUpvalue *new_upvalue(Value *slot)
{
    ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->closed = NIL_VAL;
    upvalue->next = NULL;

    return upvalue;
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
        case OBJ_CLOSURE:
            print_function(AS_CLOSURE(value)->function);
            break;
        case OBJ_FUNCTION:
            print_function(AS_FUNCTION(value));
            break;
        case OBJ_NATIVE:
            printf("<native fn");
            break;
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
        case OBJ_UPVALUE:
            printf("upvalue");
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