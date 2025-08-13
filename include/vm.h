#ifndef PEBBLE_VM_H
#define PEBBLE_VM_H

#include <stdint.h>
#include "value.h"

typedef enum {
    OP_CONST,        // push constant (operand: uint8 index)
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LOAD_GLOBAL,   // operand: const-string index
    OP_STORE_GLOBAL,  // operand: const-string index (value on stack)
    OP_PRINT,         // pop, print
    OP_POP,           // pop and discard
    OP_RETURN         // end program
} OpCode;

typedef struct {
    uint8_t *code;
    int      code_count;
    int      code_cap;

    Value   *consts;
    int      const_count;
    int      const_cap;
} Chunk;


typedef struct {
    // VM state
    uint8_t *ip;
    Value   stack[512]; int sp;

    // simple global table (name->Value) — linear, easy to replace later
    const char *names[256];
    Value values[256];
    int   gcount;
} VM;

void vm_run(Chunk *chunk);

#endif
