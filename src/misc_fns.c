#include "misc_fns.h"

#include "assert.h"
#include "debug.h"
#include "eval.h"
#include "grammar.h"
#include "lval.h"
#include "mpc.h"
#include "print.h"
#include "read.h"

int is_lval_type(Lval* lval, int type, int subtype) {
  return lval->type == type && lval->subtype == subtype;
}

Lval* macroexpand(Lenv* env, Lval* lval, int do_recurse) {
  /* Check we have non-empty list where the first expr is a symbol */
  if (is_lval_type(lval, LVAL_COLLECTION, LIST) && lval->count > 0 &&
      lval->node[0]->type == LVAL_SYMBOL) {
    /* Have a peek at the eval of that symbol */
    Lval* lval_fun = lenv_get(env, lval->node[0]);

    /* If it's a macro then eval it with the lval args */
    if (is_lval_type(lval_fun, LVAL_FUNCTION, MACRO)) {
      lval_del(lval_pop(lval, 0)); /* discard the symbol */

      /* Bind the macro with its args */
      Lval* bound_macro = eval_lambda_call(lval_fun, lval, WITHOUT_TCO);

      /* Recursively expand  */
      if (do_recurse) {
        return macroexpand(env, bound_macro, do_recurse);
      } else {
        return bound_macro;
      }
    }
  }
  return lval;
}

Lval* macroexpand_1_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "macroexpand");
  Lval* lval = lval_take(sexpr_args, 0);
  return macroexpand(env, lval, 0);
}

Lval* macroexpand_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "macroexpand");
  Lval* lval = lval_take(sexpr_args, 0);
  return macroexpand(env, lval, 1);
}

Lval* eval_fn(Lenv* env, Lval* sexpr) {
  LASSERT_NODE_COUNT(sexpr, 1, "eval");
  /* LASSERT_NODE_TYPE(sexpr, 0, LVAL_SEQ, "eval"); */
  /* LASSERT_NODE_SUBTYPE(sexpr, 0, LIST, "eval"); */
  sexpr = lval_take(sexpr, 0);
  return lval_eval(env, sexpr);
}

Lval* print_env_fn(Lenv* env, Lval* sexpr) {
  lenv_print(env);
  lval_del(sexpr);
  return make_lval_list();
}

int exit_repl = 0;

Lval* exit_fn(Lenv* e, Lval* arg_list) {
  lval_del(arg_list);
  exit_repl = 1;
  return make_lval_list();
}

/* Lval* load_fn2(Lenv* e, Lval* arg_list) { */
/*   LASSERT_NODE_COUNT(arg_list, 1, "load"); */
/*   LASSERT_NODE_SUBTYPE(arg_list, 0, STRING, "load"); */

/*   /\* Open file and check it exists *\/ */
/*   FILE* f = fopen(arg_list->node[0]->str, "rb"); */
/*   if (f == NULL) { */
/*     Lval* err = */
/*         make_lval_err("Could not load Library %s", arg_list->node[0]->str);
 */
/*     lval_del(arg_list); */
/*     return err; */
/*   } */

/*   /\* Read File Contents *\/ */
/*   fseek(f, 0, SEEK_END); */
/*   long length = ftell(f); */
/*   fseek(f, 0, SEEK_SET); */
/*   char* input = calloc(length + 1, 1); */
/*   fread(input, 1, length, f); */
/*   fclose(f); */
/*   /\* Read from input to create an S-Expr *\/ */
/*   int pos = 0; */
/*   Lval* expr = lval_read_expr(input, &pos, '\0'); */
/*   free(input); */

/*   /\* Evaluate all expressions contained in S-Expr *\/ */
/*   if (expr->type != LVAL_ERR) { */
/*     while (expr->count) { */
/*       Lval* x = lval_eval(e, lval_pop(expr, 0)); */
/*       if (x->type == LVAL_ERR) { */
/*         lval_println(x); */
/*       } */
/*       lval_del(x); */
/*     } */
/*   } else { */
/*     lval_println(expr); */
/*   } */

/*   lval_del(expr); */
/*   lval_del(arg_list); */

/*   return make_lval_list(); */
/* } */

Lval* load_fn(Lenv* env, Lval* arg_list) {
  LASSERT_NODE_COUNT(arg_list, 1, "load");
  LASSERT_NODE_SUBTYPE(arg_list, 0, STRING, "load");

  mpc_result_t result;
  if (mpc_parse_contents(arg_list->node[0]->str, Lispy, &result)) {
    /* printf("parsed again\n"); */
    /* mpc_ast_print(result.output); */

    /* parse contents into list of expressions */
    mpc_ast_t* mpc_ast = result.output;

    Lval* lval_list = read_list(make_lval_list(), mpc_ast);
    mpc_ast_delete(result.output);
    /* lval_println(lval_list); */

    /* printf("exrp count:%d\n", expressions->count); */
    Cell* cell = lval_list->list;
    while (cell) {
      Lval* x = lval_eval(env, (Lval*)cell->car);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      cell = cell->cdr;
    }
    printf("Loaded %s\n", arg_list->node[0]->str);
    lval_del(lval_list);
    return make_lval_list();
  } else {
    char* err_msg = mpc_err_string(result.error);
    mpc_err_delete(result.error);
    Lval* err = make_lval_err("Couldn't load library %s", err_msg);
    free(err_msg);
    lval_del(arg_list);
    return err;
  }
}

Lval* print_fn(Lenv* env, Lval* arg_list) {
  Cell* cell = arg_list->list;
  while (cell) {
    lval_print(cell->car);
    _putchar(' ');
    cell = cell->cdr;
  }
  _putchar('\n');
  lval_del(arg_list);
  return make_lval_list();
}

Lval* pr_fn(Lenv* env, Lval* arg_list) {
  Cell* cell = arg_list->list;
  while (cell) {
    lval_pr(cell->car);
    _putchar(' ');
    cell = cell->cdr;
  }
  _putchar('\n');
  lval_del(arg_list);
  return make_lval_list();
}

Lval* debug_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "debug");
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, NUMBER, "debug");
  Lval* lval_num = lval_pop(sexpr_args, 0);
  int num = lval_num->num;
  printf("debug = %il\n", num);
  set_debug_level((int)num);
  lval_del(sexpr_args);
  return make_lval_list();
}

void lenv_add_misc_fns(Lenv* env) {
  lenv_add_builtin(env, "eval", eval_fn, SYS);
  lenv_add_builtin(env, "print-env", print_env_fn, SYS);
  lenv_add_builtin(env, "exit", exit_fn, SYS);
  lenv_add_builtin(env, "load", load_fn, SYS);
  lenv_add_builtin(env, "print", print_fn, SYS);
  lenv_add_builtin(env, "pr", pr_fn, SYS);
  lenv_add_builtin(env, "macroexpand", macroexpand_fn, SYS);
  lenv_add_builtin(env, "macroexpand-1", macroexpand_1_fn, SYS);
  lenv_add_builtin(env, "debug", debug_fn, SYS);
}
