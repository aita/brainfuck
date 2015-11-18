#include <stdio.h>
#include <stdlib.h>

#define TAPE_LENGTH 30000
#define MAX_LOOP 1000


typedef struct bf_state {
    unsigned char tape[TAPE_LENGTH];
    unsigned char *ptr;
    void (*put_char)(struct bf_state *state, int);
    int (*get_char)(struct bf_state *state);
} bf_state_t;

typedef struct bf_program {
    unsigned char *tape;
    size_t len;
} bf_program_t;

static void run(bf_state_t *, bf_program_t *);
static void put_char(bf_state_t *, int);
static int get_char(bf_state_t *);


void
bf_run(bf_program_t *program) {
    bf_state_t state = {
        {0},
        NULL,
        put_char,
        get_char
    };
    state.ptr = &state.tape[0];
    run(&state, program);
}

static void
run(bf_state_t *s, bf_program_t *p) {
    unsigned char *c = p->tape;
    unsigned char *end = p->tape + p->len;
    int nskip = 0;
    int nloop = 0;
    unsigned char *loops[MAX_LOOP] = {0};

    for (; c != p->tape + p->len; c++) {
        // printf("%d, %d, %c\n", nskip, nloop, *c);
        switch (*c) {
            case '>':
                if (!nskip) {
                    s->ptr++;
                }
                break;
            case '<':
                if (!nskip) {
                    s->ptr--;
                }
                break;
            case '+':
                if (!nskip) {
                    (*s->ptr)++;
                }
                break;
            case '-':
                if (!nskip) {
                    (*s->ptr)--;
                }
                break;
            case '.':
                if (!nskip) {
                    s->put_char(s, *s->ptr);
                }
                break;
            case ',':
                if (!nskip) {
                    *s->ptr = (unsigned char)s->get_char(s);
                }
                break;
            case '[':
                loops[nloop++] = c;
                if (!*s->ptr) {
                    nskip++;
                }
                break;
            case ']':
                if (nloop < 1) {
                    fprintf(stderr, "] without maching [\n");
                    break;
                }
                if (*s->ptr) {
                    c = loops[nloop-1];
                } else {
                     nloop--;
                }
                if (nskip) {
                    nskip--;
                }
                break;
            case 0:
                break;
        }
    }
}

static void
put_char(bf_state_t *state, int c) {
    putchar(c);
}

static int
get_char(bf_state_t *state) {
    return getchar();
}


int
main(int argc, char **argv) {
    FILE *fp;
    size_t len;
    bf_program_t program;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", *argv);
        return 1;
    }

    if ((fp = fopen(*++argv, "r")) == NULL) {
        fprintf(stderr, "Could not open %s\n", *argv);
        return 1;
    }
    if (fseek(fp, 0, SEEK_END) < 0) {
        perror("fseek");
        return 1;
    }
    if ((program.len = ftell(fp)) < 0) {
        perror("ftell");
        return 1;
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        perror("fseek");
        return 1;
    }
    if ((program.tape = malloc(program.len)) == NULL) {
         perror("malloc");
         return 1;
    }
    if (fread(program.tape, 1, program.len, fp) < 0) {
         perror("fread");
         return 1;
    }
    if (fclose(fp) < 0) {
        perror("fclose");
        return 1;
    }

    bf_run(&program);
    free(program.tape);
    return 0;
}
