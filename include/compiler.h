#ifndef PEBBLE_COMPILER_H
#define PEBBLE_COMPILER_H

#include <stdbool.h>
#include "vm.h"      // for Chunk, opcodes
#include "lexer.h"   // token stream

bool compile_source(const char *src, Chunk *out);

#endif
