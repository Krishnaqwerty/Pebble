#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "vm.h"

#define PEBBLE_VERSION "v1.0"

static char *read_file(const char *path){
    FILE *f = fopen(path, "rb");
    if(!f) return NULL;
    fseek(f,0,SEEK_END); long n=ftell(f); fseek(f,0,SEEK_SET);
    char *buf = (char*)malloc(n+1);
    fread(buf,1,n,f); buf[n]='\0'; fclose(f);
    return buf;
}

int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file.peb>\n", argv[0]);
        return 1;
    }
    char *src = read_file(argv[1]);
    if(!src){ fprintf(stderr, "Cannot open %s\n", argv[1]); return 1; }

    if (argc > 1 && strcmp(argv[1], "--version") == 0) {
        printf("Pebble language version %s\n", PEBBLE_VERSION);
        return 0;
    }

    Chunk chunk;
    if(!compile_source(src, &chunk)){
        fprintf(stderr, "Compile failed\n");
        free(src);
        return 1;
    }

    vm_run(&chunk);

    // free constants (strings) — numbers need no free
    for(int i=0;i<chunk.const_count;i++) value_free(chunk.consts[i]);
    free(chunk.consts);
    free(chunk.code);
    free(src);
    return 0;
}
