// src/vm.c
#include "vm.h"
#include "value.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STACK_MAX 512
#define GLOBAL_MAX 256

static void push(VM *vm, Value v) {
    if (vm->sp >= STACK_MAX) {
        fprintf(stderr, "VM error: stack overflow\n");
        exit(1);
    }
    vm->stack[vm->sp++] = v;
}

static Value pop(VM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "VM error: stack underflow\n");
        exit(1);
    }
    return vm->stack[--vm->sp];
}

static uint8_t read_u8(VM *vm) {
    uint8_t b = *vm->ip;
    vm->ip += 1;
    return b;
}

/* Read a constant by index (checks bounds) */
static Value read_const(VM *vm, Chunk *chunk) {
    uint8_t idx = read_u8(vm);
    if ((int)idx >= chunk->const_count) {
        fprintf(stderr, "VM error: constant index out of range: %d (max %d)\n", idx, chunk->const_count - 1);
        exit(1);
    }
    return chunk->consts[idx];
}

/* Load global by name (linear search) */
static Value load_global(VM *vm, const char *name) {
    for (int i = 0; i < vm->gcount; ++i) {
        if (strcmp(vm->names[i], name) == 0) return vm->values[i];
    }
    fprintf(stderr, "VM error: undefined global '%s'\n", name);
    exit(1);
}

/* Store global (insert or replace) */
static void store_global(VM *vm, const char *name, Value v) {
    for (int i = 0; i < vm->gcount; ++i) {
        if (strcmp(vm->names[i], name) == 0) {
            value_free(vm->values[i]); /* free old value if string */
            vm->values[i] = v;
            return;
        }
    }
    if (vm->gcount >= GLOBAL_MAX) {
        fprintf(stderr, "VM error: too many globals\n");
        exit(1);
    }
    vm->names[vm->gcount] = name; /* pointer refers to constant memory (no free) */
    vm->values[vm->gcount] = v;
    vm->gcount++;
}

void vm_run(Chunk *chunk) {
    if (!chunk || !chunk->code || chunk->code_count == 0) {
        fprintf(stderr, "VM error: invalid or empty chunk\n");
        return;
    }

    VM vm_local = {0};
    VM *vm = &vm_local;
    vm->ip = chunk->code;
    vm->sp = 0;
    vm->gcount = 0;

    while (1) {
        uint8_t op = read_u8(vm);
        switch (op) {
            case OP_CONST: {
                Value c = read_const(vm, chunk);
                push(vm, c);
                break;
            }
            case OP_ADD: {
                Value b = pop(vm), a = pop(vm);
                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    fprintf(stderr, "VM error: + requires numbers\n");
                    exit(1);
                }
                push(vm, number_val(a.as.number + b.as.number));
                break;
            }
            case OP_SUB: {
                Value b = pop(vm), a = pop(vm);
                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    fprintf(stderr, "VM error: - requires numbers\n");
                    exit(1);
                }
                push(vm, number_val(a.as.number - b.as.number));
                break;
            }
            case OP_MUL: {
                Value b = pop(vm), a = pop(vm);
                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    fprintf(stderr, "VM error: * requires numbers\n");
                    exit(1);
                }
                push(vm, number_val(a.as.number * b.as.number));
                break;
            }
            case OP_DIV: {
                Value b = pop(vm), a = pop(vm);
                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    fprintf(stderr, "VM error: / requires numbers\n");
                    exit(1);
                }
                push(vm, number_val(a.as.number / b.as.number));
                break;
            }
            case OP_LOAD_GLOBAL: {
                Value name_val = read_const(vm, chunk);
                if (name_val.type != VAL_STRING) {
                    fprintf(stderr, "VM error: LOAD_GLOBAL expects string constant\n");
                    exit(1);
                }
                Value v = load_global(vm, name_val.as.str.chars);
                push(vm, v);
                break;
            }
            case OP_STORE_GLOBAL: {
                Value name_val = read_const(vm, chunk);
                if (name_val.type != VAL_STRING) {
                    fprintf(stderr, "VM error: STORE_GLOBAL expects string constant\n");
                    exit(1);
                }
                Value val = pop(vm);
                store_global(vm, name_val.as.str.chars, val);
                break;
            }
            case OP_PRINT: {
                Value v = pop(vm);
                value_print(v);
                printf("\n");
                /* If v is a string, we've taken ownership from constants? No — constants live separately.
                   Here we do not free v if it was pushed from a constant. If you later push temporary strings,
                   design ownership rules accordingly. */
                break;
            }
            case OP_POP: {
                (void)pop(vm);
                break;
            }
            case OP_RETURN:
                return;
            default:
                fprintf(stderr, "VM error: unknown opcode %d\n", op);
                return;
        }
    }
}
