#include "bf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *OPCODES[] = {
    "NOP",
    "ADD",
    "SUB",
    "RIGHT",
    "LEFT",
    "JZ",
    "JMP",
    "PUTC",
    "GETC",
};


static void put_char(bf_state_t *, int);
static int get_char(bf_state_t *);

bf_state_t *
bf_state_new() {
    bf_state_t *state;

    if ((state = malloc(sizeof(bf_state_t))) == NULL) {
        return NULL;
    }
    memset(state->tape, 0, sizeof(state->tape));
    state->ptr = &state->tape[0];
    state->put_char = put_char;
    state->get_char = get_char;
    return state;
}

void
bf_state_delete(bf_state_t *s) {
    free(s);
}

void
bf_run(bf_state_t *s, bf_program_t *p) {
    bf_opcode_t *op = &p->opcodes[0];
    bf_opcode_t *end = op + p->len;

    for (; op != end;) {
        unsigned int n;
        long offset;

        switch (*op++) {
            case NOP:
                break;
            case RIGHT:
                n = *op++;
                s->ptr += n;
                break;
            case LEFT:
                n = *op++;
                s->ptr -= n;
                break;
            case ADD:
                n = *op++;
                (*s->ptr) += n;
                break;
            case SUB:
                n = *op++;
                (*s->ptr) -= n;
                break;
            case PUTC:
                s->put_char(s, *s->ptr);
                break;
            case GETC:
                *s->ptr = (unsigned char)s->get_char(s);
                break;
            case JZ:
                offset = (long)*op++;
                if (!*s->ptr) {
                    op += offset;
                }
                break;
            case JMP:
                offset = (long)*op++;
                op += offset;
                break;
            default:
                fprintf(stderr, "invalid opcode: %ld\n", *op);
                return;
        }
    }
}

bf_program_t *
bf_compile(const char *c) {
    unsigned int nloops = 0;
    unsigned int nlabels = 0;
    long offset = 0;
    long labels[MAX_LABELS];
    bf_program_t *p;
    bf_opcode_t *op;


    if ((p = malloc(sizeof(bf_program_t))) == NULL) {
        return NULL;
    }
    memset(p->opcodes, NOP, sizeof(p->opcodes));

    op = &p->opcodes[0];
    for (;;) {
        int n;

        if (offset >= PROGRAM_SIZE) {
            fprintf(stderr, "the program is too long.\n");
            return NULL;
        }

        switch (*c++) {
            case '>':
                *op++ = RIGHT;
                for(n = 1; *c == '<' && n < OPCODE_MAX; n++, c++);
                *op++ = n;
                offset += 2;
                break;
            case '<':
                *op++ = LEFT;
                for(n = 1; *c == '>' && n < OPCODE_MAX; n++, c++);
                *op++ = n;
                offset += 2;
                break;
            case '+':
                *op++ = ADD;
                for(n = 1; *c == '+' && n < OPCODE_MAX; n++, c++);
                *op++ = n;
                offset += 2;
                break;
            case '-':
                *op++ = SUB;
                for(n = 1; *c == '-' && n < OPCODE_MAX; n++, c++);
                *op++ = n;
                offset += 2;
                break;
            case '.':
                *op++ = PUTC;
                offset++;
                break;
            case ',':
                *op++ = GETC;
                offset++;
                break;
            case '[':
                *op = JZ;
                labels[nloops++] = offset;
                op += 2;
                offset += 2;
                break;
            case ']':
                if (nloops < 1) {
                    fprintf(stderr, "bad loop\n");
                    return NULL;
                }
                nloops--;
                *op++ = JMP;
                *op++ = labels[nloops] - offset - 2;
                p->opcodes[labels[nloops]+1] = offset - labels[nloops];
                offset += 2;
                break;
            case 0:
                goto exit;
            default:
                break;
        }
    }
exit:
    p->len = offset;
    return p;
}

void
bf_disasm(bf_program_t *p) {
    long off = 0;
    bf_opcode_t *op = p->opcodes;

    for (; op != op + p->len; op++) {
        switch (*op) {
            case LEFT:
            case RIGHT:
            case ADD:
            case SUB:
            case JMP:
            case JZ:
                printf("%ld\t%s(%ld): %ld\n", off, OPCODES[*op], *op, *(op+1));
                op++;
                off += 2;
                break;
            case NOP:
            case PUTC:
            case GETC:
                printf("%ld\t%s\n", off, OPCODES[*op]);
                off++;
                break;
            default:
                printf("%ld\tinvalid opcode: %ld\n", off, *op);
                off++;
                break;
        }
    }
}

void
bf_program_delete(bf_program_t *p) {
    free(p);
}

static void
put_char(bf_state_t *state, int c) {
    putchar(c);
}

static int
get_char(bf_state_t *state) {
    return getchar();
}
