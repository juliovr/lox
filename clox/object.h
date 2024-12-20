#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_STRING(value)    is_obj_type(value, OBJ_STRING)

#define AS_STRING(value)    ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_STRING,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj *next;
};

struct ObjString {
    Obj obj;
    int length;
    char chars[];
};

#ifndef STRINGS_FLEXIBLE_ARRAY_MEMBER
ObjString *take_string(char *chars, int length);
#endif

ObjString *copy_string(const char *chars, int length);
ObjString *concatenate_object_strings(ObjString *a, ObjString *b);
void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif