#include "misc_fns.h"

#include "assert.h"
#include "debug.h"
#include "eval.h"
#include "grammar.h"
#include "lval.h"
#include "mpc.h"
#include "print.h"
#include "read.h"

bool is_lval_type(Lval* lval, int type, int subtype) {
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

Lval* print_env_fn(Lenv* e, Lval* sexpr) {
  lenv_print(e);
  lval_del(sexpr);
  return make_lval_list();
}

bool exit_repl = false;

Lval* exit_fn(Lenv* e, Lval* sexpr) {
  lval_del(sexpr);
  exit_repl = true;
  return make_lval_list();
}

Lval* load_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "load");
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, STRING, "load");

  mpc_result_t result;
  if (mpc_parse_contents(sexpr_args->node[0]->str, Lispy, &result)) {
    /* printf("parsed again\n"); */
    /* mpc_ast_print(result.output); */

    /* parse contents into list of expressions */
    mpc_ast_t* t = result.output;

    Lval* expressions =
        read_expressions(make_lval_list(), t->children, t->children_num);
    mpc_ast_delete(result.output);
    /* printf("exrp count:%d\n", expressions->count); */
    while (expressions->count) {
      Lval* expr = lval_pop(expressions, 0);
      /* lval_println(expr); */
      /* lval_del(expr); */

      Lval* x = lval_eval(env, expr);
      if (x->type == LVAL_ERR) {
        lval_println(x);
      }
      lval_del(x);
    }
    lval_del(expressions);

    printf("Loaded %s\n", sexpr_args->node[0]->str);
    lval_del(sexpr_args);
    return make_lval_list();
  } else {
    char* err_msg = mpc_err_string(result.error);
    mpc_err_delete(result.error);
    Lval* err = make_lval_err("Couldn't load library %s", err_msg);
    free(err_msg);
    lval_del(sexpr_args);
    return err;
  }
}

Lval* print_fn(Lenv* env, Lval* sexpr_args) {
  for (int i = 0; i < sexpr_args->count; ++i) {
    lval_print(sexpr_args->node[i]);
    _putchar(' ');
  }
  _putchar('\n');
  lval_del(sexpr_args);

  return make_lval_list();
}

Lval* pr_fn(Lenv* env, Lval* sexpr_args) {
  for (int i = 0; i < sexpr_args->count; ++i) {
    lval_pr(sexpr_args->node[i]);
    _putchar(' ');
  }
  _putchar('\n');
  lval_del(sexpr_args);

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
