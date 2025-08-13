#ifndef PEBBLE_LEXER_H
#define PEBBLE_LEXER_H

typedef enum {
    TK_EOF, TK_NUMBER, TK_STRING, TK_ID, TK_OP, TK_NEWLINE
} TokenKind;

typedef struct {
    TokenKind kind;
    const char *start;
    int length;
    double number;
} Token;

typedef struct {
    const char *src;
    int pos;
} Lexer;

void  lex_init(Lexer *L, const char *src);
Token lex_next(Lexer *L);

#endif
