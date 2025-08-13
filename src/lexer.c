#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

static Token make(TokenKind k, const char *st, int len){ Token t; t.kind=k; t.start=st; t.length=len; t.number=0; return t; }

static void skip_ws(Lexer *L){
    const char *s=L->src;
    int p=L->pos;
    for(;;){
        while(s[p]==' '||s[p]=='\t'||s[p]=='\r') p++;
        if(s[p]=='/' && s[p+1]=='/'){ while(s[p] && s[p]!='\n') p++; }
        else break;
    }
    L->pos=p;
}

void lex_init(Lexer *L, const char *src){ L->src=src; L->pos=0; }

Token lex_next(Lexer *L){
    const char *s = L->src;
    skip_ws(L);
    int p = L->pos;
    if(!s[p]) return make(TK_EOF, s+p, 0);
    if(s[p]=='\n'){ L->pos=p+1; return make(TK_NEWLINE, s+p, 1); }
    if(isdigit((unsigned char)s[p])){
        int q=p; while(isdigit((unsigned char)s[q])||s[q]=='.') q++;
        Token t=make(TK_NUMBER, s+p, q-p);
        char buf[64]; int n=q-p; if(n>63)n=63; memcpy(buf,s+p,n); buf[n]='\0';
        t.number = atof(buf);
        L->pos=q; return t;
    }
    if(isalpha((unsigned char)s[p])||s[p]=='_'){
        int q=p; while(isalnum((unsigned char)s[q])||s[q]=='_') q++;
        Token t=make(TK_ID, s+p, q-p);
        L->pos=q; return t;
    }
    if(s[p]=='"'){
        int q=++p; while(s[q] && s[q]!='"') q++;
        Token t=make(TK_STRING, s+p, q-p);
        L->pos = s[q]=='"'? q+1 : q;
        return t;
    }
    Token t=make(TK_OP, s+p, 1); L->pos=p+1; return t;
}
