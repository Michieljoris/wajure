#ifndef __ASSERT_H_
#define __ASSERT_H_

#define LASSERT(sexpr, cond, fmt, ...)             \
  if (!(cond)) {                                   \
    Lval* err = make_lval_err(fmt, ##__VA_ARGS__); \
    lval_del(sexpr);                               \
    return err;                                    \
  }

#define LASSERT_NODE_COUNT(sexpr, expected_count, fn_name) \
  LASSERT(sexpr, sexpr->count == expected_count,           \
          "Function '%s' passed wrong number of args, "    \
          "got %i, expected %i",                           \
          fn_name, sexpr->count, expected_count);

#define LASSERT_NODE_TYPE(sexpr, index, expected_type, fn_name)        \
  LASSERT(sexpr, sexpr->node[index]->type == expected_type,            \
          "Function '%s' passed incorrect type for arg %d, "           \
          "got %s, expected %s",                                       \
          fn_name, index, lval_type_to_name(sexpr->node[index]->type), \
          lval_type_to_name(expected_type));

#define LASSERT_IS_LIST_TYPE(lval, index, fn_name)                            \
  int type = lval->node[index]->type;                                         \
  LASSERT(lval,                                                               \
          !(type == LVAL_SEXPR || type == LVAL_VECTOR || type == LVAL_MAP),   \
          "parameter %d for %s should be a list, vector or map, but is a %s", \
          index, fn_name, lval_type_to_name(type));

#endif  // __ASSERT_H_
