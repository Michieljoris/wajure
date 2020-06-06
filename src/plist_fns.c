#include "assert.h"
#include "env.h"
#include "io.h"
#include "lval.h"
#include "print.h"

Lval* cons_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 2, "cons");
  LASSERT_NODE_SUBTYPE(sexpr_args, 1, PLIST, "cons");
  Lval* x = lval_pop(sexpr_args, 0);
  Lval* plist = lval_pop(sexpr_args, 0);
  Lval* plist2 = make_lval_plist();
  Cell* cell_for_x = make_cell();

  plist2->list = cell_for_x;
  cell_for_x->car = x;
  cell_for_x->cdr = plist->list;
  lval_del(sexpr_args);
  return plist2;
}

Lval* pfirst_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "first");
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, PLIST, "first");
  Lval* plist = lval_pop(sexpr_args, 0);
  Cell* cell = plist->list;
  Lval* ret = NULL;
  if (cell) {
    ret = cell->car;
    if (ret->subtype == PLIST) {
      // Make an another plist to point at car of plist
      ret = make_lval_plist();
      ret->list = ((Lval*)(cell->car))->list;
    }
  }
  lval_del(sexpr_args);
  return ret ? ret : make_lval_list();
}

Lval* plist_fn(Lenv* env, Lval* sexpr_args) {
  Lval* ret = make_lval_plist();
  Cell* cell = NULL;
  if (sexpr_args->count) {
    cell = ret->list = make_cell();
    ret->list->car = lval_pop(sexpr_args, 0);
  }
  while (sexpr_args->count) {
    cell->cdr = make_cell();
    cell->cdr->car = lval_pop(sexpr_args, 0);
    cell = cell->cdr;
  }
  lval_del(sexpr_args);
  return ret;
}

Lval* prest_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 1, "rest");
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, PLIST, "rest");
  Lval* plist = lval_pop(sexpr_args, 0);
  Lval* plist2 = make_lval_plist();
  if (plist->list) plist2->list = plist->list->cdr;
  lval_del(plist);
  lval_del(sexpr_args);
  return plist2;
}

Lval* pconcat_fn(Lenv* env, Lval* sexpr_args) {
  LASSERT_NODE_COUNT(sexpr_args, 2, "rest");
  LASSERT_NODE_SUBTYPE(sexpr_args, 0, PLIST, "rest");
  LASSERT_NODE_SUBTYPE(sexpr_args, 1, PLIST, "rest");
  Lval* plist1 = lval_pop(sexpr_args, 0);
  Lval* plist2 = lval_pop(sexpr_args, 0);
  Lval* ret = NULL;
  if (!plist1->list)
    ret = plist2;
  else if (!plist2->list)
    ret = plist1;
  else {
    ret = make_lval_plist();
    Cell* cell = make_cell();
    ret->list = cell;
    Cell* last_cell = copy_cells(plist1->list, cell, NULL);
    last_cell->cdr = plist2->list;
  }
  lval_del(sexpr_args);
  return ret;
}

void lenv_add_plist_fns(Lenv* env) {
  lenv_add_builtin(env, "cons", cons_fn, SYS);
  lenv_add_builtin(env, "pfirst", pfirst_fn, SYS);
  lenv_add_builtin(env, "plist", plist_fn, SYS);
  lenv_add_builtin(env, "prest", prest_fn, SYS);
  lenv_add_builtin(env, "pconcat", pconcat_fn, SYS);
}
