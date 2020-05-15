#include "lval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval* lval_err(char* m)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

void lval_del(lval* v)
{
    switch (v->type) {
    case LVAL_NUM:
        break;
    case LVAL_SYM:
        free(v->sym);
        break;
    case LVAL_ERR:
        free(v->err);
        break;
    case LVAL_SEXPR:
        for (int i = 0; i < v->count; ++i) {
            lval_del(v->cell[i]);
        }
        free(v->cell);
    }
}
