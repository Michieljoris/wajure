#include "eval.h"
#include "lval.h"
#include <stdlib.h>
#include <string.h>

static lval* lval_pop(lval* v, int i)
{
    lval* x = v->cell[i];
    memmove(&v->cell[i], &v->cell[i + 1],
        sizeof(lval*) * (v->count - i - 1));
    v->count--;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

static lval* lval_take(lval* v, int i)
{
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

static lval* builtin_op(lval* operands, char* op)
{
    for (int i = 0; i < operands->count; ++i) {
        if (operands->cell[i]->type != LVAL_NUM) {
            lval_del(operands);
            return lval_err("Cannot operate on non-number");
        }
    }
    lval* result = lval_pop(operands, 0);

    if ((strcmp(op, "-") == 0) && operands->count == 0) {
        result->num = -result->num;
    }

    while (operands->count > 0) {
        lval* operand = lval_pop(operands, 0);
        if (strcmp(op, "+") == 0) {
            result->num += operand->num;
        }
        if (strcmp(op, "-") == 0) {
            result->num -= operand->num;
        }
        if (strcmp(op, "*") == 0) {
            result->num *= operand->num;
        }
        if (strcmp(op, "/") == 0) {
            if (operand->num == 0) {
                lval_del(result);
                lval_del(operand);
                result = lval_err("Division boperand zero");
                break;
            }
            result->num /= operand->num;
        }

        lval_del(operand);
    }
    lval_del(operands);
    return result;
}

lval* lval_eval(lval* v);
static lval* lval_eval_sexpr(lval* v)
{
    /* eval children */
    for (int i = 0; i < v->count; ++i) {
        v->cell[i] = lval_eval(v->cell[i]);
    }
    /* error checking */
    for (int i = 0; i < v->count; ++i) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }
    /* empty expr */
    if (v->count == 0) {
        return v;
    }
    /* single expr */
    if (v->count == 1) {
        return lval_take(v, 0);
    }
    /* first element should be a symbol  */
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f);
        lval_del(v);
        return lval_err("sexpr doesn't start with a symbol");
    }
    /* call builtin with operator */
    lval* result = builtin_op(v, f->sym);
    lval_del(f);
    return result;
}

lval* lval_eval(lval* v)
{
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(v);
    }
    return v;
}
