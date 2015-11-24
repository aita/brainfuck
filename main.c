#include "bf.h"
#include <stdio.h>
#include <stdlib.h>

static char *
read_file(const char *path) {
    FILE *fp;
    char *s;
    size_t len;

    if ((fp = fopen(path, "r")) == NULL) {
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) < 0) {
        return NULL;
    }
    if ((len = ftell(fp)) < 0) {
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        return NULL;
    }
    if ((s = malloc(len+1)) == NULL) {
         return NULL;
    }
    s[len] = 0;
    if (fread(s, 1, len, fp) < 0) {
         return NULL;
    }
    if (fclose(fp) < 0) {
        return NULL;
    }
    return s;
}


int
main(int argc, char **argv) {
    /*
    bf_state_t *state;
    bf_program_t *program;
    */
    char *codes;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", *argv);
        return 1;
    }
    if ((codes = read_file(*++argv)) == NULL) {
         return 1;
    }

    bf_x86_compile(codes);
    free(codes);

    /*
    program = bf_compile(codes);
    free(codes);
    bf_disasm(program);
    state = bf_state_new();
    bf_run(state, program);
    bf_program_delete(program);
    bf_state_delete(state);
    */

    return 0;
}
