#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include "debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2


void *reallocate(void *pointer, size_t old_size, size_t new_size)
{
    vm.bytes_allocated += new_size - old_size;

    if (new_size > old_size) {
#ifdef DEBUG_STRESS_GC
        collect_garbage();
#endif

        if (vm.bytes_allocated > vm.next_gc) {
            collect_garbage();
        }
    }

    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL) {
        printf("Could not allocate memory. Terminating...");
        exit(1);
    }

    return result;
}

/*
 * Mark the object as "seen" by the GC by setting the is_marked to true and adding to the vm gray stack.
 */
void mark_object(Obj *object)
{
    if (object == NULL) return;
    if (object->is_marked) return;

#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void *)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif

    object->is_marked = true;

    if (vm.gray_capacity < vm.gray_count + 1) {
        vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
        vm.gray_stack = (Obj **)realloc(vm.gray_stack, sizeof(Obj **)*vm.gray_capacity);

        if (vm.gray_stack == NULL) exit(1);
    }

    vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(Value value)
{
    if (IS_OBJ(value)) mark_object(AS_OBJ(value));
}

static void mark_array(ValueArray *array)
{
    for (int i = 0; i < array->count; i++) {
        mark_value(array->values[i]);
    }
}


// @Optimize: An easy optimization we could do in markObject() is to skip adding strings and native functions to the gray stack at all since we know they don’t need to be processed. Instead, they could darken from white straight to black.
static void blacken_object(Obj *object)
{
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void *)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_CLOSURE:
            ObjClosure *closure = (ObjClosure *)object;
            mark_object((Obj *)closure->function);
            for (int i = 0; i < closure->upvalue_count; i++) {
                mark_object((Obj *)closure->upvalues[i]);
            }
            break;
        case OBJ_FUNCTION:
            ObjFunction *function = (ObjFunction *)object;
            mark_object((Obj *)function->name);
            mark_array(&function->chunk.constants);
            break;
        case OBJ_UPVALUE:
            mark_value(((ObjUpvalue *)object)->closed);
            break;
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
    }
}

static void free_object(Obj *object)
{
#ifdef DEBUG_LOG_GC
    printf("%p free type %d\n", (void *)object, object->type);
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
            FREE(ObjClosure, object);
        } break;
        case OBJ_FUNCTION: {
            ObjFunction *function = (ObjFunction *)object;
            free_chunk(&function->chunk);
            FREE(ObjFunction, object);
        } break;
        case OBJ_NATIVE: {
            FREE(ObjNative, object);
        } break;
        case OBJ_STRING: {
            ObjString *string = (ObjString *)object;
            FREE_ARRAY(char, string->chars, string->length);
            FREE(ObjString, object);
        } break;
        case OBJ_UPVALUE: {
            ObjClosure *closure = (ObjClosure *)object;
            FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalue_count);
            FREE(ObjUpvalue, object);
        } break;
    }
}

static void mark_roots()
{
    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
        mark_value(*slot);
    }

    for (int i = 0; i < vm.frame_count; i++) {
        mark_object((Obj *)vm.frames[i].closure);
    }

    for (ObjUpvalue *upvalue = vm.open_upvalues; upvalue != NULL; upvalue = upvalue->next) {
        mark_object((Obj *)upvalue);
    }

    mark_table(&vm.globals);
    mark_compiler_roots();
}

/*
 * Traverse all gray objects in the vm gray stack and mark them as black (by decreasing the gray_count).
 */
static void trace_references()
{
    while (vm.gray_count > 0) {
        Obj *object = vm.gray_stack[--vm.gray_count];
        blacken_object(object);
    }
}

static void sweep()
{
    Obj *previous = NULL;
    Obj *object = vm.objects;
    while (object != NULL) {
        if (object->is_marked) {
            object->is_marked = false;

            // Black nodes, don't do anything.
            previous = object;
            object = object->next;
        } else {
            // White nodes.
            Obj *unreached = object;
            object = object->next;

            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }

            free_object(unreached);
        }
    }
}

void collect_garbage()
{
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm.bytes_allocated;
#endif

    mark_roots();
    trace_references();

    // The strings are not marked during trace_references() because they would be marked as they all are "used" in the table.
    // Instead only walks through the other stacks and heaps to find references to string objects. Here, the unused strings are removed from the string pool table.
    table_remove_white(&vm.strings);

    sweep();

    vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
    printf("   collected %zu bytes (from %zu to %zu) next at %zu\n", 
            (before - vm.bytes_allocated), before, vm.bytes_allocated, vm.next_gc);
#endif
}

void free_objects()
{
    Obj *object = vm.objects;
    while (object != NULL) {
        Obj *next = object->next;
        free_object(object);
        object = next;
    }

    free(vm.gray_stack);
}
