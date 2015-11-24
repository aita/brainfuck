#include "bf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
bf_x86_compile(const char *c) {
    int label = 0;
    int nloops = 0;
    int loops[MAX_LOOPS];

    printf(".section .data\n");
    printf("\t.comm tape, %d\n", TAPE_LENGTH);
    printf("\n");

    printf(".section .text\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("\tpushl %%ebp\n");
    printf("\tmovl %%esp, %%ebp\n");
    printf("\tmovl $tape, %%edi\n");

    for (;;) {
        int n;

        switch (*c++) {
            case '>':
                for(n = 1; *c == '<' && n < OPCODE_MAX; n++, c++);
                printf("\tsubl $%d, %%edi\n", n);
                break;
            case '<':
                for(n = 1; *c == '>' && n < OPCODE_MAX; n++, c++);
                printf("\taddl $%d, %%edi\n", n);
                break;
            case '+':
                for(n = 1; *c == '+' && n < OPCODE_MAX; n++, c++);
                printf("\taddb $%d, (%%edi)\n", n);
                break;
            case '-':
                for(n = 1; *c == '-' && n < OPCODE_MAX; n++, c++);
                printf("\tsubb $%d, (%%edi)\n", n);
                break;
            case '.':
                printf("\tsubl $0x4, %%esp\n");
                printf("\tmovsbl (%%edi), %%eax\n");
                printf("\tmovl %%eax, (%%esp)\n");
                printf("\tcall putchar\n");
                printf("\taddl $0x4, %%esp\n");
                break;
            case ',':
                printf("\tcall getchar\n");
                printf("\tmovl %%al, (%%edi)\n");
                break;
            case '[':
                loops[nloops] = label;
                printf(".L%d:\n", label++);
                printf("\tcmpb $0, (%%edi)\n");
                printf("\tjz .L%d\n", label++);
                nloops++;
                break;
            case ']':
                if (nloops < 1) {
                    fprintf(stderr, "bad loop\n");
                    return;
                }
                nloops--;
                printf("\tjmp .L%d\n", loops[nloops]);
                printf(".L%d:\n", loops[nloops]+1);
                break;
            case 0:
                printf("\tmovl $0, %%eax\n");
                printf("\tmovl %%ebp, %%esp\n");
                printf("\tpopl %%ebp\n");
                printf("\tret\n");
                goto exit;
            default:
                break;
        }
    }
exit:
    return;
}

