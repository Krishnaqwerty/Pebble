#include "compiler.h"
#include "value.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static Chunk *chunk;
static Lexer L;
static Token cur;


static void ensure_code_cap(int need) {
    if (chunk->code_cap >= need) return;
    int newcap = chunk->code_cap ? chunk->code_cap : 64;
    while (newcap < need) newcap *= 2;
    uint8_t *p = (uint8_t*)realloc(chunk->code, (size_t)newcap);
    if (!p) { fprintf(stderr, "Out of memory (code buffer)\n"); exit(1); }
    chunk->code = p;
    chunk->code_cap = newcap;
}


static void ensure_const_cap(int need) {
    if (chunk->const_cap >= need) return;
    int newcap = chunk->const_cap ? chunk->const_cap : 64;
    while (newcap < need) newcap *= 2;
    Value *p = (Value*)realloc(chunk->consts, sizeof(Value) * (size_t)newcap);
    if (!p) { fprintf(stderr, "Out of memory (consts)\n"); exit(1); }
    chunk->consts = p;
    chunk->const_cap = newcap;
}


static void emit(uint8_t b) {
    ensure_code_cap(chunk->code_count + 1);
    chunk->code[chunk->code_count++] = b;
}


static int add_const(Value v) {
    ensure_const_cap(chunk->const_count + 1);
    chunk->consts[chunk->const_count] = v;
    return chunk->const_count++;
}

static void emit_const_num(double x){ int i=add_const(number_val(x)); emit(OP_CONST); emit((uint8_t)i); }
static int  emit_const_str(const char *s){ int i=add_const(string_val(s)); emit(OP_CONST); emit((uint8_t)i); return i; }

static void advance(){ cur = lex_next(&L); }
static bool match(TokenKind k){ if(cur.kind==k){ advance(); return true;} return false; }
static bool match_op(char c){ if(cur.kind==TK_OP && cur.length==1 && cur.start[0]==c){ advance(); return true;} return false; }

static int is_kw(const char *kw){
    return cur.kind==TK_ID && (int)strlen(kw)==cur.length && strncmp(cur.start, kw, cur.length)==0;
}

static char *tok_to_cstr(Token t){
    char *s = (char*)malloc(t.length+1);
    memcpy(s, t.start, t.length);
    s[t.length]='\0';
    return s;
}

static void error(const char *msg){ fprintf(stderr, "Compile error: %s\n", msg); exit(1); }

// fwd decl
static void statement();
static void expression();
static void term();
static void factor();

// program := { statement } EOF
static void program(){
    advance();
    while(cur.kind != TK_EOF){
        if(cur.kind==TK_NEWLINE){ advance(); continue; }
        statement();
    }
    emit(OP_RETURN);
}

// statement := "say" expression NEWLINE?
//            | "let" ID "=" expression NEWLINE?
//            | expression NEWLINE?
static void statement(){
    if(is_kw("say")){
        advance();
        expression();
        emit(OP_PRINT);
        match(TK_NEWLINE);
        return;
    }
    if(is_kw("let")){
        advance();
        if(cur.kind != TK_ID) error("expected identifier after let");
        char *name = tok_to_cstr(cur); advance();
        if(!match_op('=')) error("expected = after identifier");
        expression(); // leaves value on stack
        // store into global
        int sidx = add_const(string_val(name)); free(name);
        emit(OP_STORE_GLOBAL); emit((uint8_t)sidx);
        match(TK_NEWLINE);
        return;
    }
    // bare expression (evaluate and print) — nice for quick testing
    expression();
    emit(OP_PRINT);
    match(TK_NEWLINE);
}

// expression := term (('+'|'-') term)*
static void expression(){
    term();
    while(cur.kind==TK_OP && (cur.start[0]=='+' || cur.start[0]=='-')){
        char op = cur.start[0]; advance();
        term();
        emit(op=='+' ? OP_ADD : OP_SUB);
    }
}

// term := factor (('*'|'/') factor)*
static void term(){
    factor();
    while(cur.kind==TK_OP && (cur.start[0]=='*' || cur.start[0]=='/')){
        char op = cur.start[0]; advance();
        factor();
        emit(op=='*' ? OP_MUL : OP_DIV);
    }
}

// factor := NUMBER | STRING | ID | '(' expression ')'
static void factor(){
    if(cur.kind == TK_NUMBER){ emit_const_num(cur.number); advance(); return; }
    if(cur.kind == TK_STRING){
        char *s = tok_to_cstr(cur);
        emit_const_str(s);
        free(s);
        advance();
        return;
    }
    if(cur.kind == TK_ID){
        char *name = tok_to_cstr(cur); advance();
        int sidx = add_const(string_val(name)); free(name);
        emit(OP_LOAD_GLOBAL); emit((uint8_t)sidx);
        return;
    }
    if(match_op('(')){ expression(); if(!match_op(')')) error("expected ')'"); return; }
    error("unexpected token in expression");
}

/* compile_source: top-level entry
   - 'out' is the Chunk provided by the caller (main)
   - set the compiler's static chunk pointer to out so emit/add_const write into it
   - zero-init the structure so caps/pointers are sane
*/
bool compile_source(const char *src, Chunk *out){
    if (!out) return false;
    /* zero init the chunk memory so counts and caps are 0 */
    memset(out, 0, sizeof(*out));

    /* point the compiler's internal chunk pointer to the caller's chunk */
    chunk = out;

    /* (optional) initialize capacity fields explicitly (already zeroed above) */
    chunk->code = NULL;
    chunk->code_count = 0;
    chunk->code_cap = 0;
    chunk->consts = NULL;
    chunk->const_count = 0;
    chunk->const_cap = 0;

    /* initialize lexer and start parsing/emitting */
    lex_init(&L, src);
    program();   /* your top-level parse/emit function */


    fprintf(stderr, "[DEBUG] compile_source done: code_count=%d code_cap=%d const_count=%d const_cap=%d\n",
        chunk->code_count, chunk->code_cap, chunk->const_count, chunk->const_cap);


    return true;
}

