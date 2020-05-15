#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"
#include <editline/history.h>
#include <editline/readline.h>

typedef struct lval {
    int type;
    char* sym;
    long num;
    char* err;
    /* list of lval */
    int count;
    struct lval** cell;
} lval;

/* lval types */
enum {
    LVAL_NUM,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR
};

lval* lval_num(long x)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_sym(char* s)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

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

lval* lval_read_num(mpc_ast_t* t)
{
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_add(lval* v, lval* x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval* lval_read(mpc_ast_t* t)
{
    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    }
    if (strstr(t->tag, "symbol")) {
        return lval_sym(t->contents);
    }
    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr")) {
        x = lval_sexpr();
    }
    for (int i = 0; i < t->children_num; ++i) {
        char* contents = t->children[i]->contents;
        if (strcmp(contents, "(") == 0) {
            continue;
        }
        if (strcmp(contents, ")") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}

void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v)
{
    switch (v->type) {
    case LVAL_NUM:
        printf("%li", v->num);
        break;
    case LVAL_ERR:
        printf("Error: %s", v->err);
        break;
    case LVAL_SYM:
        printf("%s", v->sym);
        break;
    case LVAL_SEXPR:
        lval_expr_print(v, '(', ')');
        break;
    }
}

void lval_expr_print(lval* v, char open, char close)
{
    putchar(open);
    for (int i = 0; i < v->count; ++i) {
        lval_print(v->cell[i]);
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_println(lval* v)
{
    lval_print(v);
    putchar('\n');
}

lval* lval_pop(lval* v, int i)
{
    lval* x = v->cell[i];
    memmove(&v->cell[i], &v->cell[i + 1],
        sizeof(lval*) * (v->count - i - 1));
    v->count--;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x
}

lval* lval_take(lval* v, int i)
{
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval* lval_eval_sexpr(lval* v);
lval* lval_eval(lval* v)
{
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(v);
    }
    return v;
}

lval* builtin_op(lval* operands, char* op)
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

lval* lval_eval_sexpr(lval* v)
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

int main(int argc, char** argv)
{
    /* Create Some Parsers */
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    /* Define them with the following Language */
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                     \
    number   : /-?[0-9]+/ ;                                             \
    symbol   : '+' | '-' | '*' | '/' ;                                  \
    sexpr    : '(' <expr>* ')' ;                                        \
    expr     : <number> | <symbol> | <sexpr> ;                          \
    lispy    : /^/ <expr>* /$/ ;                                        \
  ",
        Number,
        Symbol,
        Sexpr,
        Expr,
        Lispy);

    /* Print Version and Exit Information */
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    /* In a never ending loop */
    while (1) {

        /* Output our prompt and get input*/
        char* input = readline("lispy>");
        /* printf("INPUT:%s\n", input); */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            /* printf("Succes\n"); */
            /* lval result = eval(r.output); */
            lval* ast = lval_read(r.output);
            lval* result = lval_eval(ast);
            lval_println(result);
            lval_del(result);
            mpc_ast_delete(r.output);
        } else {
            printf("Error\n");
            mpc_err_print(r.error);
            mpc_err_print(r.error);
        }

        /* Add input to history */
        add_history(input);

        /* Echo input back to user */
        /* printf("No you're a %s\n", input); */

        free(input);
    }
    /* Undefine and Delete our Parsers */
    mpc_cleanup(4, Number, Symbol, Sexpr, Expr, Lispy);

    return 0;
}

/* int number_of_nodes(mpc_ast_t *t) { */
/*   printf("Tag: %s\n", t->tag); */
/*   printf("Contents: %s\n", t->contents); */
/*   printf("Number of children: %i\n", t->children_num); */
/*   if (t->children_num == 0) { */
/*     return 1; */
/*   } */
/*   if (t->children_num >= 1) { */
/*     int total = 1; */
/*     for (int i = 0; i < t->children_num; i++) { */
/*       total = total + number_of_nodes(t->children[i]); */
/*     } */
/*     return total; */
/*   } */
/*   return 0; */
/* } */
