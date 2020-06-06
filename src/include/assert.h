#ifndef __ASSERT_H_
#define __ASSERT_H_

#define LASSERT(sexpr, cond, fmt, ...)             \
  if (!(cond)) {                                   \
    Lval* err = make_lval_err(fmt, ##__VA_ARGS__); \
    lval_del(sexpr);                               \
    return err;                                    \
  }

#define LASSERT_LIST_COUNT(lval_list, expected_count, fn_name) \
  int __count = list_count(lval_list->list);                   \
  LASSERT(lval_list, __count == expected_count,                \
          "Function '%s' passed wrong number of args, "        \
          "got %i, expected %i",                               \
          fn_name, __count, expected_count);

#define LASSERT_TYPE(lval_list, lval, expected_type, fn_name, index) \
  LASSERT(lval_list, lval->type == expected_type,                    \
          "Function '%s' passed incorrect type for arg %d, "         \
          "got %s, expected %s",                                     \
          fn_name, index, lval_type_to_name2(lval),                  \
          lval_type_to_name(expected_type));

#define LASSERT_NODE_COUNT(sexpr, expected_count, fn_name) \
  LASSERT(sexpr, sexpr->count == expected_count,           \
          "Function '%s' passed wrong number of args, "    \
          "got %i, expected %i",                           \
          fn_name, sexpr->count, expected_count);

#define LASSERT_NODE_TYPE(sexpr, index, expected_type, fn_name)   \
  LASSERT(sexpr, sexpr->node[index]->type == expected_type,       \
          "Function '%s' passed incorrect type for arg %d, "      \
          "got %s, expected %s",                                  \
          fn_name, index, lval_type_to_name2(sexpr->node[index]), \
          lval_type_to_name(expected_type));

#define LASSERT_NODE_SUBTYPE(sexpr, index, expected_type, fn_name) \
  LASSERT(sexpr, sexpr->node[index]->subtype == expected_type,     \
          "Function '%s' passed incorrect type for arg %d, "       \
          "got %s, expected %s",                                   \
          fn_name, index, lval_type_to_name2(sexpr->node[index]),  \
          lval_type_to_name(expected_type));

#define LASSERT_IS_LIST_TYPE(lval, index, fn_name)                            \
  int type = lval->node[index]->type;                                         \
  LASSERT(lval, type == LVAL_COLLECTION,                                      \
          "parameter %d for %s should be a list, vector or map, but is a %s", \
          index, fn_name, lval_type_to_name2(lval->node[index]));

#define LASSERT_LVAL_IS_LIST_TYPE(lval, fn_name)                 \
  int type = lval->type;                                         \
  LASSERT(lval, type == LVAL_COLLECTION,                         \
          "Expecting a list, vector or map, but got a %s in %s", \
          lval_type_to_name2(lval), fn_name);

#endif  // __ASSERT_H_
