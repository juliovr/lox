#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "value.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_CLOSURE(value)   is_obj_type(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)  is_obj_type(value, OBJ_FUNCTION)
#define IS_NATIVE(value)    is_obj_type(value, OBJ_NATIVE)
#define IS_STRING(value)    is_obj_type(value, OBJ_STRING)

#define AS_CLOSURE(value)   ((ObjClosure *)AS_OBJ(value))
#define AS_FUNCTION(value)  ((ObjFunction *)AS_OBJ(value))
#define AS_NATIVE(value)    ((ObjNative *)AS_OBJ(value))
#define AS_STRING(value)    ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
} ObjType;

struct Obj {
    ObjType type;
    bool is_marked;
    struct Obj *next;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalue_count;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef Value (*NativeFn)(int arg_count, Value *args);

typedef struct {
    Obj obj;
    int arity;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj;
    int length;
    u32 hash;
    char chars[];
};

typedef struct {
    Obj obj;
    Value *location;
    Value closed;
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalue_count;
} ObjClosure;

#ifndef STRINGS_FLEXIBLE_ARRAY_MEMBER
ObjString *take_string(char *chars, int length);
#endif

ObjClosure *new_closure(ObjFunction *function);
ObjFunction *new_function();
ObjNative *new_native(NativeFn function, int arity);
ObjString *copy_string(const char *chars, int length);
ObjUpvalue *new_upvalue(Value *slot);
ObjString *concatenate_object_strings(ObjString *a, ObjString *b);
void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif