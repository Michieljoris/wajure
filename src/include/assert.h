#ifndef __ASSERT_H_
#define __ASSERT_H_
#include "lib.h"
#include "lval.h"

#define LASSERT(sexpr, cond, fmt, ...)             \
  if (!(cond)) {                                   \
    Lval* err = make_lval_err(fmt, ##__VA_ARGS__); \
    return err;                                    \
  }

#define LASSERT_TYPE(fn_name, arg_list, index, expected_type, \
                     expected_subtype, lval)                  \
  LASSERT(arg_list,                                           \
          lval->type == expected_type &&                      \
              lval->subtype == (long int)expected_subtype,    \
          "Function '%s' passed incorrect type for arg %d, "  \
          "got %s, expected %s",                              \
          fn_name, index, lval_type_to_name(lval),            \
          lval_type_constant_to_name(expected_type));

#define LASSERT_LVAL_LIST_COUNT(lval_list, expected_count, fn_name) \
  int __count = list_count(lval_list->list);                        \
  LASSERT(lval_list, __count == expected_count,                     \
          "Function '%s' passed wrong number of args, "             \
          "got %i, expected %i",                                    \
          fn_name, __count, expected_count);

#define LASSERT_LIST_COUNT(lval_list, list, expected_count, fn_name) \
  int __count = list_count(list);                                    \
  LASSERT(lval_list, __count == expected_count,                      \
          "Function '%s' passed wrong number of args, "              \
          "got %i, expected %i",                                     \
          fn_name, __count, expected_count);

#define LASSERT_LVAL_IS_LIST_TYPE(lval, fn_name)                 \
  int type = lval->type;                                         \
  LASSERT(lval, type == LVAL_COLLECTION,                         \
          "Expecting a list, vector or map, but got a %s in %s", \
          lval_type_to_name(lval), fn_name);

//-----------------------------------------------------------------------

#define LASSERT_NODE_COUNT(sexpr, expected_count, fn_name) \
  LASSERT(sexpr, sexpr->count == expected_count,           \
          "Function '%s' passed wrong number of args, "    \
          "got %i, expected %i",                           \
          fn_name, sexpr->count, expected_count);

#define LASSERT_NODE_TYPE(sexpr, index, expected_type, fn_name)  \
  LASSERT(sexpr, sexpr->node[index]->type == expected_type,       \
          "Function '%s' passed incorrect type for arg %d, "      \
          "got %s, expected %s",                                  \
          fn_name, index, lval_type_to_name(sexpr->node[index]), \
          lval_type_constant_to_nameexpected_type));

#define LASSERT_NODE_SUBTYPE(sexpr, index, expected_type, fn_name) \
  LASSERT(sexpr, sexpr->node[index]->subtype == expected_type,     \
          "Function '%s' passed incorrect type for arg %d, "       \
          "got %s, expected %s",                                   \
          fn_name, index, lval_type_to_name(sexpr->node[index]),   \
          lval_type_constant_to_name(expected_type));

#define LASSERT_IS_LIST_TYPE(lval, index, fn_name)                            \
  int type = lval->node[index]->type;                                         \
  LASSERT(lval, type == LVAL_COLLECTION,                                      \
          "parameter %d for %s should be a list, vector or map, but is a %s", \
          index, fn_name, lval_type_to_name(lval->node[index]));

#endif  // __ASSERT_H_
