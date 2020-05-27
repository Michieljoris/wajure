#include "misc_fns.h"

#include "assert.h"
#include "eval.h"
#include "grammar.h"
#include "lval.h"
#include "mpc.h"
#include "print.h"
#include "read.h"

Lval* eval_fn(Lenv* env, Lval* sexpr) {
  LASSERT_NODE_COUNT(sexpr, 1, "eval");
  LASSERT_NODE_TYPE(sexpr, 0, LVAL_SEQ, "eval");
  LASSERT_NODE_SUBTYPE(sexpr, 0, LIST, "eval");
  sexpr = lval_take(sexpr, 0);
  return lval_eval(env, sexpr);
}

Lval* print_env_fn(Lenv* e, Lval* sexpr) {
  lenv_print(e);
  lval_del(sexpr);
  return make_lval_sexpr();
}

bool exit_repl = false;

Lval* exit_fn(Lenv* e, Lval* sexpr) {
  lval_del(sexpr);
  exit_repl = true;
  return make_lval_sexpr();
}

Lval* load_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "load");
  LASSERT_NODE_TYPE(sexpr_args, 0, LVAL_STR, "load");

  mpc_result_t result;
  if (mpc_parse_contents(sexpr_args->node[0]->str, Lispy, &result)) {
    /* printf("parsed again\n"); */
    /* mpc_ast_print(result.output); */

    /* parse contents into list of expressions */
    mpc_ast_t* t = result.output;

    Lval* expressions =
        read_expressions(make_lval_sexpr(), t->children, t->children_num);
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

    lval_del(sexpr_args);
    return make_lval_sexpr();
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
    putchar(' ');
  }
  putchar('\n');
  lval_del(sexpr_args);

  return make_lval_sexpr();
}

void lenv_add_misc_fns(Lenv* env) {
  lenv_add_builtin(env, "eval", eval_fn, SYS);
  lenv_add_builtin(env, "print-env", print_env_fn, SYS);
  lenv_add_builtin(env, "exit", exit_fn, SYS);
  lenv_add_builtin(env, "load", load_fn, SYS);
  lenv_add_builtin(env, "print", print_fn, SYS);
}
