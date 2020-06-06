#include "math_fns.h"

#include "assert.h"
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lval.h"

static Lval* op_fn(Lenv* env, char* operator, Lval * arg_list) {
  Cell* cell = arg_list->list;
  int result;
  Lval* lval_num;
  switch (*operator) {
    case '+':
      result = 0;
      break;
    case '*':
      result = 1;
      break;
    case '-':
    case '/':
      if (!cell) {
        return make_lval_err(
            "Math operation %s needs at least one argument", operator);
      }
      lval_num = cell->car;
      if (!cell->cdr) {
        if (lval_num->subtype != NUMBER) {
          return make_lval_err("Expected number but got %s",
                               lval_type_to_name2(cell->car));
        }
        if (*operator== '/' && lval_num->num == 0)
          return make_lval_err("Division by number zero");
        return *operator== '-' ? make_lval_num(-lval_num->num)
                               : make_lval_num(1 / lval_num->num);
      }
      result = lval_num->num;
      cell = cell->cdr;
      break;
    default:
      return make_lval_err("Unsupported math operation: %s", operator);
  }

  while (cell) {
    lval_num = cell->car;
    if (lval_num->subtype != NUMBER) {
      return make_lval_err("Expected number but got %s",
                           lval_type_to_name2(cell->car));
    }
    switch (*operator) {
      case '+':
        result += lval_num->num;
        break;
      case '*':
        result *= lval_num->num;
        break;
      case '-':
        result -= lval_num->num;
        break;
      case '/':
        if (lval_num->num == 0) {
          lval_del(lval_num);
          return make_lval_err("Division by number zero");
          break;
        }
        result /= lval_num->num;
        break;
    }
    cell = cell->cdr;
  }
  return make_lval_num(result);
}

Lval* add_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "+", arg_list); }
Lval* sub_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "-", arg_list); }
Lval* mul_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "*", arg_list); }
Lval* div_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "/", arg_list); }

// TODO: make proper assert macros for arg_lists
/* LASSERT_NODE_COUNT(sexpr_args, 2, operator_str);                 \ */
/* LASSERT_NODE_SUBTYPE(sexpr_args, 0, NUMBER, operator_str);       \ */
/* LASSERT_NODE_SUBTYPE(sexpr_args, 1, NUMBER, operator_str);       \ */
#define MATH_FN(fn_name, operator_str, operator)                        \
  Lval* fn_name(Lenv* env, Lval* arg_list) {                            \
    Lval* first_arg = (Lval*)(arg_list->list->car);                     \
    Lval* second_arg = ((Lval*)(arg_list->list->cdr->car));             \
    Lval* num = make_lval_num(first_arg->num operator second_arg->num); \
    lval_del(arg_list);                                                 \
    return num;                                                         \
  }

MATH_FN(gt_fn, ">", >);
MATH_FN(lt_fn, "<", <);
MATH_FN(lte_fn, "<=", <=);
MATH_FN(gte_fn, ">=", >=);

int lval_eq(Lval* x, Lval* y) {
  if (x->type != y->type) {
    return 0;
  }

  switch (x->type) {
    case LVAL_SYMBOL:
      return (_strcmp(x->sym, y->sym) == 0);
    case LVAL_COLLECTION:
      return x == y ? 1 : 0;
      // TODO: make proper comparison!!!
      /* if (x->count != y->count) { */
      /*   return 0; */
      /* } */
      /* for (int i = 0; i < x->count; ++i) { */
      /*   if (!lval_eq(x->node[i], y->node[i])) { */
      /*     return 0; */
      /*   } */
      /* } */
      /* return 1; */
    case LVAL_LITERAL: {
      case NUMBER:
        return (x->num == y->num);
      case STRING:
        return (_strcmp(x->str, y->str) == 0);
      default:
        printf("Warning: comparing instances of type '%s' is not implemented\n",
               lval_type_to_name2(x));
        return 0;
    }
    case LVAL_FUNCTION:
      if (x->fun || y->fun) {
        return x->fun == y->fun;
      } else {
        return lval_eq(x->params, y->params) && lval_eq(x->body, y->body);
      }
    case LVAL_ERR:
      return (_strcmp(x->err, y->err) == 0);
  }
  printf("Warning: comparing instances of type '%s' is not implemented\n",
         lval_type_to_name2(x));
  return 0;
}

Lval* cmp_fn(Lenv* env, Lval* arg_list, char* operator) {
  /* LASSERT_NODE_COUNT(arg_list, 2, operator); */
  int result;
  if (_strcmp(operator, "=") == 0) {
    result = lval_eq(list_first(arg_list->list), list_nth(arg_list->list, 1));
  }

  if (_strcmp(operator, "not=") == 0) {
    result = !lval_eq(list_first(arg_list->list), list_nth(arg_list->list, 1));
  }
  lval_del(arg_list);
  return make_lval_num(result);
}

Lval* eq_fn(Lenv* env, Lval* arg_list) { return cmp_fn(env, arg_list, "="); }

Lval* not_eq_fn(Lenv* env, Lval* arg_list) {
  return cmp_fn(env, arg_list, "not=");
}

void lenv_add_math_fns(Lenv* env) {
  lenv_add_builtin(env, "+", add_fn, SYS);
  lenv_add_builtin(env, "-", sub_fn, SYS);
  lenv_add_builtin(env, "*", mul_fn, SYS);
  lenv_add_builtin(env, "/", div_fn, SYS);

  lenv_add_builtin(env, ">", gt_fn, SYS);
  lenv_add_builtin(env, "<", lt_fn, SYS);
  lenv_add_builtin(env, ">=", gte_fn, SYS);
  lenv_add_builtin(env, "<=", lte_fn, SYS);

  lenv_add_builtin(env, "=", eq_fn, SYS);
  lenv_add_builtin(env, "not=", not_eq_fn, SYS);
}
