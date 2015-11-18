#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAPE_LENGTH 30000
#define MAX_LOOP 1000
#define PROGRAM_SIZE 100000

enum {
    NOP,
    ADD,
    SUB,
    RIGHT,
    LEFT,
    LOOP,
    ENDLOOP,
    EXIT,
    PUTC,
    GETC,
    NUM_OPCODES
};

typedef unsigned char bf_opcode_t;

typedef struct bf_state {
    unsigned char tape[TAPE_LENGTH];
    unsigned char *ptr;
    void (*put_char)(struct bf_state *state, int);
    int (*get_char)(struct bf_state *state);
} bf_state_t;

typedef struct bf_program {
    // unsigned char *opcodes;
    // size_t len;
    bf_opcode_t opcodes[PROGRAM_SIZE];
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
    int nskip = 0;
    int nloop = 0;
    bf_opcode_t *loops[MAX_LOOP] = {0};
    bf_opcode_t *op = &p->opcodes[0];

    for (;;) {
        int n;

        switch (*op++) {
            case NOP:
                break;
            case RIGHT:
                n = *op++;
                if (!nskip) {
                    s->ptr += n;
                }
                break;
            case LEFT:
                n = *op++;
                if (!nskip) {
                    s->ptr -= n;
                }
                break;
            case ADD:
                n = *op++;
                if (!nskip) {
                    (*s->ptr) += n;
                }
                break;
            case SUB:
                n = *op++;
                if (!nskip) {
                    (*s->ptr) -= n;
                }
                break;
            case PUTC:
                if (!nskip) {
                    s->put_char(s, *s->ptr);
                }
                break;
            case GETC:
                if (!nskip) {
                    *s->ptr = (unsigned char)s->get_char(s);
                }
                break;
            case LOOP:
                loops[nloop++] = op;
                if (!*s->ptr) {
                    nskip++;
                }
                break;
            case ENDLOOP:
                if (nloop < 1) {
                    fprintf(stderr, "bad loop\n");
                    return;
                }
                if (*s->ptr) {
                    op = loops[nloop-1];
                } else {
                     nloop--;
                }
                if (nskip) {
                    nskip--;
                }
                break;
            case EXIT:
                return;
            default:
                fprintf(stderr, "invalid opcode: %d\n", *op);
                return;
        }
    }
}

static bf_program_t *
bf_compile(const char *c) {
    bf_program_t *p;
    bf_opcode_t *op;

    if ((p = malloc(sizeof(bf_program_t))) == NULL) {
        return NULL;
    }
    memset(p->opcodes, NOP, sizeof(p->opcodes));

    op = &p->opcodes[0];
    for (;;) {
        int n;

        switch (*c++) {
            case '>':
                *op++ = RIGHT;
                for(n = 1; *c == '<' && n < 256; n++, c++);
                *op++ = n;
                break;
            case '<':
                *op++ = LEFT;
                for(n = 1; *c == '>' && n < 256; n++, c++);
                *op++ = n;
                break;
            case '+':
                *op++ = ADD;
                for(n = 1; *c == '+' && n < 256; n++, c++);
                *op++ = n;
                break;
            case '-':
                *op++ = SUB;
                for(n = 1; *c == '-' && n < 256; n++, c++);
                *op++ = n;
                break;
            case '.':
                *op++ = PUTC;
                break;
            case ',':
                *op++ = GETC;
                break;
            case '[':
                *op++ = LOOP;
                break;
            case ']':
                *op++ = ENDLOOP;
                break;
            case 0:
                *op = EXIT;
                goto exit;
            default:
                break;
        }
    }

exit:
    return p;
}

void
bf_program_delete(bf_program_t *p) {
    // free(p->opcodes);
    // free(p);
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
    bf_program_t *program;
    char *codes;

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
    if ((len = ftell(fp)) < 0) {
        perror("ftell");
        return 1;
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        perror("fseek");
        return 1;
    }
    if ((codes = malloc(len+1)) == NULL) {
         perror("malloc");
         return 1;
    }
    codes[len] = 0;
    if (fread(codes, 1, len, fp) < 0) {
         perror("fread");
         return 1;
    }
    if (fclose(fp) < 0) {
        perror("fclose");
        return 1;
    }

    program = bf_compile(codes);
    free(codes);
    bf_run(program);
    bf_program_delete(program);

    return 0;
}
