#include "value.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char *dup_c(const char *s){ size_t n=strlen(s); char *r=(char*)malloc(n+1); memcpy(r,s,n+1); return r; }

Value number_val(double x){ Value v; v.type=VAL_NUMBER; v.as.number=x; return v; }
Value string_val(const char *s){ Value v; v.type=VAL_STRING; v.as.str.length=(int)strlen(s); v.as.str.chars=dup_c(s); return v; }

void value_free(Value v){ if(v.type==VAL_STRING && v.as.str.chars) free(v.as.str.chars); }

void value_print(Value v){
    if(v.type==VAL_NUMBER) printf("%g", v.as.number);
    else if(v.type==VAL_STRING) printf("%s", v.as.str.chars);
}

bool value_truthy(Value v){
    if(v.type==VAL_NUMBER) return v.as.number != 0.0;
    if(v.type==VAL_STRING) return v.as.str.length != 0;
    return false;
}
