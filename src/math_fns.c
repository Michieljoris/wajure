#include "math_fns.h"

#include "assert.h"
#include "env.h"
#include "io.h"
#include "iter.h"
#include "lib.h"
#include "lval.h"

static Lval* op_fn(Lenv* env, char* operator, Lval * arg_list) {
  scoped_iter Cell* i = iter_new(arg_list);
  Lval* arg = iter_next(i);
  long int result;
  switch (*operator) {
    case '+':
      result = 0;
      break;
    case '*':
      result = 1;
      break;
    case '-':
    case '/':
      if (!arg) {
        return make_lval_err(
            "Math operation %s needs at least one argument", operator);
      }
      if (!iter_peek(i)) {
        if (arg->subtype != NUMBER) {
          return make_lval_err("Expected number but got %s",
                               lval_type_to_name(arg));
        }
        if (*operator== '/' && arg->num == 0) {
          return make_lval_err("Division by number zero");
        }
        return *operator== '-' ? make_lval_num(-arg->num)
                               : make_lval_num(1 / arg->num);
      }
      result = arg->num;
      break;
    default:
      return make_lval_err("Unsupported math operation: %s", operator);
  }
  while (arg) {
    if (arg->subtype != NUMBER) {
      return make_lval_err("Expected number but got %s",
                           lval_type_to_name(arg));
    }
    switch (*operator) {
      case '+':
        result += arg->num;
        break;
      case '*':
        result *= arg->num;
        break;
      case '-':
        result -= arg->num;
        break;
      case '/':
        if (arg->num == 0) {
          return make_lval_err("Division by number zero");
          break;
        }
        result /= arg->num;
        break;
    }
    arg = iter_next(i);
  }
  return make_lval_num(result);
}

Lval* add_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "+", arg_list); }
Lval* sub_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "-", arg_list); }
Lval* mul_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "*", arg_list); }
Lval* div_fn(Lenv* e, Lval* arg_list) { return op_fn(e, "/", arg_list); }

#define MATH_FN(fn_name, operator_str, operator)                        \
  Lval* fn_name(Lenv* env, Lval* arg_list) {                            \
    ITER_NEW_N(operator_str, 2)                                         \
    ITER_NEXT                                                           \
    LASSERT_TYPE(fn_name, arg_list, 0, LVAL_LITERAL, NUMBER, arg)       \
    Lval* first_arg = arg;                                              \
    ITER_NEXT                                                           \
    LASSERT_TYPE(fn_name, arg_list, 1, LVAL_LITERAL, NUMBER, arg)       \
    Lval* second_arg = arg;                                             \
    Lval* num = make_lval_num(first_arg->num operator second_arg->num); \
    ITER_END                                                            \
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
      if (list_count(x->head) != list_count(y->head)) {
        return 0;
      }
      Cell* xl = x->head;
      Cell* yl = y->head;
      while (xl) {
        if (!lval_eq(xl->car, yl->car)) {
          return 0;
        }
        xl = xl->cdr;
        yl = yl->cdr;
      }
      return 1;
    case LVAL_LITERAL: {
      case NUMBER:
        return (x->num == y->num);
      case STRING:
        return (_strcmp(x->str, y->str) == 0);
      default:
        printf("Warning: comparing instances of type '%s' is not implemented\n",
               lval_type_to_name(x));
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
         lval_type_to_name(x));
  return 0;
}

Lval* cmp_fn(Lenv* env, Lval* arg_list, char* operator) {
  ITER_NEW_N(operator, 2)
  ITER_NEXT
  Lval* first_arg = arg;
  ITER_NEXT
  Lval* second_arg = arg;
  int result;
  if (_strcmp(operator, "=") == 0) {
    result = lval_eq(first_arg, second_arg);
  }
  if (_strcmp(operator, "not=") == 0) {
    result = !lval_eq(first_arg, second_arg);
  }
  ITER_END
  return make_lval_num(result);
}

Lval* eq_fn(Lenv* env, Lval* arg_list) { return cmp_fn(env, arg_list, "="); }

Lval* not_eq_fn(Lenv* env, Lval* arg_list) {
  return cmp_fn(env, arg_list, "not=");
}

Builtin math_builtins[11] = {

    {"+", add_fn}, {"-", sub_fn},       {"*", mul_fn},  {"/", div_fn},
    {">", gt_fn},  {"<", lt_fn},        {">=", gte_fn}, {"<=", lte_fn},
    {"=", eq_fn},  {"not=", not_eq_fn}, {NIL}

};
