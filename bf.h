#include <limits.h>

#define TAPE_LENGTH 30000
#define MAX_LABELS 1000
#define PROGRAM_SIZE 100000
#define OPCODE_MAX ULONG_MAX

enum {
    NOP,
    ADD,
    SUB,
    RIGHT,
    LEFT,
    JZ,
    JMP,
    PUTC,
    GETC,
    EXIT,
    NUM_OPCODES,
};

extern const char *OPCODES[];

typedef unsigned long bf_opcode_t;

typedef struct bf_state {
    unsigned char tape[TAPE_LENGTH];
    unsigned char *ptr;
    void (*put_char)(struct bf_state *state, int);
    int (*get_char)(struct bf_state *state);
} bf_state_t;

typedef struct bf_program {
    bf_opcode_t opcodes[PROGRAM_SIZE];
} bf_program_t;


bf_state_t *bf_state_new();
void bf_state_delete(bf_state_t *);
bf_program_t *bf_compile(const char *);
void bf_assemble(bf_program_t *);
void bf_disasm(bf_program_t *);
void bf_run(bf_state_t *, bf_program_t *);
void bf_program_delete(bf_program_t *);
