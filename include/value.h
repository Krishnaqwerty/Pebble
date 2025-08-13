#ifndef PEBBLE_VALUE_H
#define PEBBLE_VALUE_H

#include <stdbool.h>

typedef enum { VAL_NUMBER, VAL_STRING } ValueType;

typedef struct {
    char *chars;
    int length;
} PString;

typedef struct {
    ValueType type;
    union { double number; PString str; } as;
} Value;

Value number_val(double x);
Value string_val(const char *s);
void  value_free(Value v);
void  value_print(Value v);
bool  value_truthy(Value v);

#endif
