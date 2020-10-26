#include "math_fns.h"

#include "assert.h"
#include "io.h"
#include "iter.h"
#include "lib.h"
#include "list.h"
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
        if (*operator== '/' && arg->data.num == 0) {
          return make_lval_err("Division by number zero");
        }
        return *operator== '-' ? make_lval_num(-arg->data.num)
                               : make_lval_num(1 / arg->data.num);
      }
      result = arg->data.num;
      arg = iter_next(i);
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
        result += arg->data.num;
        break;
      case '*':
        result *= arg->data.num;
        break;
      case '-':
        result -= arg->data.num;
        break;
      case '/':
        if (arg->data.num == 0) {
          return make_lval_err("Division by number zero");
          break;
        }
        result /= arg->data.num;
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

#define MATH_FN(fn_name, operator_str, operator)                   \
  Lval* fn_name(Lenv* env, Lval* arg_list) {                       \
    ITER_NEW_N(operator_str, 2)                                    \
    ITER_NEXT                                                      \
    LASSERT_TYPE(fn_name, arg_list, 0, LVAL_LITERAL, NUMBER, arg)  \
    Lval* first_arg = arg;                                         \
    ITER_NEXT                                                      \
    LASSERT_TYPE(fn_name, arg_list, 1, LVAL_LITERAL, NUMBER, arg)  \
    Lval* second_arg = arg;                                        \
    Lval* bool = first_arg->data.num operator second_arg->data.num \
                     ? make_lval_true()                            \
                     : make_lval_false();                          \
    ITER_END                                                       \
    return bool;                                                   \
  }

MATH_FN(gt_fn, ">", >);
MATH_FN(lt_fn, "<", <);
MATH_FN(lte_fn, "<=", <=);
MATH_FN(gte_fn, ">=", >=);

int lval_eq(Lval* x, Lval* y) {
  /* if (x->hash != y->hash) return 0; */
  if (x->type != y->type) return 0;

  switch (x->type) {
    case LVAL_SYMBOL:
      return (_strcmp(x->data.str, y->data.str) == 0);
    case LVAL_COLLECTION:
      if (list_count(x->data.head) != list_count(y->data.head)) {
        return 0;
      }
      Cell* xl = x->data.head;
      Cell* yl = y->data.head;
      while (xl) {
        if (!lval_eq(xl->car, yl->car)) {
          return 0;
        }
        xl = xl->cdr;
        yl = yl->cdr;
      }
      return 1;
    case LVAL_LITERAL:
      switch (x->subtype) {
        case NUMBER:
          return (x->data.num == y->data.num);
        case KEYWORD:
        case STRING:
          return (_strcmp(x->data.str, y->data.str) == 0);
        case LNIL:
        case LTRUE:
        case LFALSE:
          return x->subtype == y->subtype;
        default:
          printf(
              "Warning: comparing instances of type '%s' is not implemented\n",
              lval_type_to_name(x));
          return 0;
      }
    case LVAL_FUNCTION:
      /* if (x->fun || y->fun) { */
      /*   return x->fun == y->fun; */
      /* } else { */
      /*   return lval_eq(x->params, y->params) && lval_eq(x->body, y->body); */
      /* } */
    case LVAL_ERR:
      return x == y;
      /* return (_strcmp(x->str, y->str) == 0); */
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
  return result ? make_lval_true() : make_lval_false();
}

Lval* eq_fn(Lenv* env, Lval* arg_list) { return cmp_fn(env, arg_list, "="); }

Lval* not_eq_fn(Lenv* env, Lval* arg_list) {
  return cmp_fn(env, arg_list, "not=");
}

CFn math_c_fns[] = {

    {"+", add_fn, "add_fn", 2, 1},
    {"-", sub_fn, "sub_fn", 2, 1},
    {"*", mul_fn, "mul_fn", 2, 1},
    {"/", div_fn, "div_fn", 2, 1},
    {">", gt_fn, "gt_fn", 2, 1},
    {"<", lt_fn, "lt_fn", 2, 1},
    {">=", gte_fn, "gte_fn", 2, 1},
    {"<=", lte_fn, "lte_fn", 2, 1},
    {"=", eq_fn, "eq_fn", 2, 1},
    {"not=", not_eq_fn, "not_eq_fn", 2, 1},
    {NIL}

};
