#include "misc_fns.h"

#include "assert.h"
#include "debug.h"
#include "eval.h"
#include "grammar.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "lval.h"
#include "mpc.h"
#include "mpc_read.h"
#include "print.h"
#include "read.h"

int is_lval_type(Lval* lval, int type, int subtype) {
  return lval->type == type && lval->subtype == subtype;
}

Lval* macroexpand(Lenv* env, Lval* lval, int do_recurse) {
  /* Check we have non-empty list where the first expr is a symbol */
  if (is_lval_type(lval, LVAL_COLLECTION, LIST) && list_count(lval->head) > 0 &&
      ((Lval*)(lval->head->car))->type == LVAL_SYMBOL) {
    /* Have a peek at the eval of that symbol */
    Lval* lval_fun = lenv_get(env, lval->head->car);

    /* If it's a macro then eval it with the lval args */
    if (is_lval_type(lval_fun, LVAL_FUNCTION, MACRO)) {
      Lval* arg_list = make_lval_list();
      arg_list->head = lval->head->cdr;
      /* Bind the macro with its args */
      Lval* bound_macro = eval_lambda_call(lval_fun, arg_list, WITHOUT_TCO);
      // release fun and args
      if (bound_macro->type == LVAL_ERR) {
        return bound_macro;
      }
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

Lval* macroexpand_1_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("macroexpand-1", 1)
  ITER_NEXT
  Lval* lval = arg;
  ITER_END
  return macroexpand(env, lval, 0);
}

Lval* macroexpand_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("macroexpand", 1)
  ITER_NEXT
  Lval* lval = arg;
  ITER_END
  return macroexpand(env, lval, 1);
}

Lval* eval_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("eval", 1)
  ITER_NEXT
  Lval* lval = arg;
  ITER_END
  return lval_eval(env, lval);
}

Lval* print_env_fn(Lenv* env, Lval* arg_list) {
  lenv_print(env);
  return make_lval_list();
}

int exit_repl = 0;

Lval* exit_fn(Lenv* e, Lval* arg_list) {
  exit_repl = 1;
  return make_lval_list();
}

Lval* load_fn(Lenv* env, Lval* arg_list) {
  printf("\nload_fn: ");
  ITER_NEW_N("load", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, STRING)

  /* Open file and check it exists */
  FILE* f = fopen(arg->str, "rb");
  if (f == NULL) {
    Lval* err = make_lval_err("Could not load Library %s", arg->str);
    return err;
  }
  ITER_END

  /* Read File Contents */
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* input = calloc(length + 1, 1);
  fread(input, 1, length, f);
  fclose(f);
  /* Read from input to create an S-Expr */
  int pos = 0;
  set_debug_level(1);
  int lval_count1 = get_free_slot_count(LVAL);
  int cell_count1 = get_free_slot_count(CELL);
  Lval* lval_list = lval_read_list(input, &pos, '\0');
  free(input);

  set_debug_level(1);
  print_mempool_free_all();
  lval_println(lval_list);

  print_mempool_free_all();
  printf("DONE READING FILE ==================== ");
  int lval_count2 = get_free_slot_count(LVAL);
  int cell_count2 = get_free_slot_count(CELL);
  int lval_count_extra = lval_count2 - lval_count1;
  int cell_count_extra = cell_count2 - cell_count1;
  printf("Extra: LVAL: %d | CELL: %d\n", lval_count_extra, cell_count_extra);
  printf("Evalling lval_list: ");
  lval_println(lval_list);
  /* Evaluate all expressions contained in S-Expr */
  Lval* result = NIL;
  if (lval_list->type != LVAL_ERR) {
    Cell* i = iter_new(lval_list);
    Lval* lval = iter_next(i);

    while (lval) {
      release(result);
      printf("\nEvalling: ");
      lval_println(lval);
      result = lval_eval(env, lval);  // EVAL
      printf("\nDone evalling lval_list\n");
      if (result->type == LVAL_ERR) {
        lval_println(result);
      }
      lval = iter_next(i);
    }

    printf("Releasing lval_list: ");
    release(lval_list);
    iter_end(i);
  } else {
    lval_println(lval_list);
    iter_end(i);
  }

  print_mempool_free_all();
  printf("Done evalling =================== ");
  int lval_count3 = get_free_slot_count(LVAL);
  int cell_count3 = get_free_slot_count(CELL);
  int lval_count_extra2 = lval_count3 - lval_count1;
  int cell_count_extra2 = cell_count3 - cell_count1;
  printf("Extra: LVAL: %d | CELL: %d\n", lval_count_extra2, cell_count_extra2);
  printf("\n");
  return result ? result : make_lval_list();
}

/* Lval* mpc_load_fn(Lenv* env, Lval* arg_list) { */
/*   ITER_NEW_N("load", 1) */
/*   ITER_NEXT_TYPE(LVAL_LITERAL, STRING) */

/*   mpc_result_t result; */
/*   Lval* lval_str = arg; */
/*   ITER_END */
/*   if (mpc_parse_contents(lval_str->str, Lispy, &result)) { */
/*     /\* printf("parsed again\n"); *\/ */
/*     /\* mpc_ast_print(result.output); *\/ */

/*     /\* parse contents into list of expressions *\/ */
/*     mpc_ast_t* mpc_ast = result.output; */

/*     Lval* lval_list = read_list(make_lval_list(), mpc_ast); */
/*     mpc_ast_delete(result.output); */
/*     /\* lval_println(lval_list); *\/ */

/*     /\* printf("exrp count:%d\n", expressions->count); *\/ */
/*     Cell* cell = lval_list->head; */
/*     while (cell) { */
/*       Lval* x = lval_eval(env, (Lval*)cell->car); */
/*       if (x->type == LVAL_ERR) { */
/*         lval_println(x); */
/*       } */
/*       cell = cell->cdr; */
/*     } */
/*     printf("Loaded %s\n", lval_str->str); */
/*     return make_lval_list(); */
/*   } else { */
/*     char* err_msg = mpc_err_string(result.error); */
/*     mpc_err_delete(result.error); */
/*     Lval* err = make_lval_err("Couldn't load library %s", err_msg); */
/*     free(err_msg); */
/*     return err; */
/*   } */
/* } */

Lval* print_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW("print")
  ITER_NEXT
  while (arg) {
    lval_print(arg);
    putchar(' ');
    ITER_NEXT
  }
  ITER_END
  putchar('\n');
  return make_lval_list();
}

Lval* pr_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW("print")
  ITER_NEXT
  while (arg) {
    lval_pr(arg);
    putchar(' ');
    ITER_NEXT
  }
  ITER_END
  putchar('\n');
  return make_lval_list();
}

Lval* debug_fn(Lenv* env, Lval* arg_list) {
  ITER_NEW_N("debug", 1)
  ITER_NEXT_TYPE(LVAL_LITERAL, NUMBER)
  int num = arg->num;
  ITER_END
  printf("debug = %il\n", num);
  set_debug_level((int)num);
  return make_lval_list();
}

Builtin misc_builtins[10] = {

    {"eval", eval_fn},
    {"print-env", print_env_fn},
    {"exit", exit_fn},
    {"load", load_fn},
    /* {"mpc_load", mpc_load_fn}, */
    {"print", print_fn},
    {"pr", pr_fn},
    {"macroexpand", macroexpand_fn},
    {"macroexpand-1", macroexpand_1_fn},
    {"debug", debug_fn},
    {NULL}

};
