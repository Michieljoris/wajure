#include "lval.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "io.h"
#include "lispy_mempool.h"

Lval* make_lval_fun(lbuiltin func, char* func_name, int subtype) {
  Lval* lval = lalloc(LVAL);
  lval->func_name = calloc(1, strlen(func_name) + 1);
  strcpy(lval->func_name, func_name);
  lval->type = LVAL_FUN;
  lval->subtype = subtype;
  lval->fun = func;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_lambda(Lenv* env, Lval* formals, Lval* body, int subtype) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_FUN;
  lval->subtype = subtype;
  lval->bindings = lenv_new();
  lval->bindings->parent_env = env;
  lval->formals = formals;
  lval->body = body;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_num(long x) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_NUM;
  lval->num = x;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_sym(char* s) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_SYM;
  lval->sym = calloc(1, strlen(s) + 1);
  strcpy(lval->sym, s);
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_str(char* s) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_STR;
  lval->str = calloc(1, strlen(s) + 1);
  strcpy(lval->str, s);
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_sexpr(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_SEQ;
  lval->subtype = LIST;
  lval->cell = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_vector(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_SEQ;
  lval->subtype = VECTOR;
  lval->cell = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_map(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_SEQ;
  lval->subtype = MAP;
  lval->cell = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_quote(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_QUOTE;
  lval->cell = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_err(char* fmt, ...) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_ERR;
  lval->subtype = SYS;
  va_list va;
  va_start(va, fmt);
  lval->err = calloc(1, 512);
  vsnprintf(lval->err, 511, fmt, va);
  lval->err = realloc(lval->err, strlen(lval->err) + 1);

  va_end(va);
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_exception(char* msg) {
  Lval* lval = make_lval_err(msg);
  lval->subtype = USER;
  lval->tco_env = NULL;
  return lval;
}

char* lval_type_to_name2(Lval* lval) {
  switch (lval->type) {
    case LVAL_NUM:
      return "Number";
    case LVAL_SYM:
      return "Symbol";
    case LVAL_SEQ:
      switch (lval->subtype) {
        case PLIST:
          return "Plist (seq)";
        case LIST:
          return "List (seq)";
        case VECTOR:
          return "Vector (seq)";
        case MAP:
          return "Map (seq)";
        default:
          return "unknown SEQ subtype";
      }
    case LVAL_ERR:
      return "Error";
    case LVAL_FUN:
      switch (lval->subtype) {
        case SYS:
          return "System Function";
        case LAMBDA:
          return "User Function";
        case SPECIAL:
          return "Special form";
        case MACRO:
          return "Macro";
      }
    case LVAL_STR:
      return "String";
    default:
      return "Unknown";
  }
}

char* lval_type_to_name(int t) {
  switch (t) {
    case LVAL_NUM:
      return "Number";
    case LVAL_SYM:
      return "Symbol";
    case LVAL_SEQ:
      return "Seq";
    case LVAL_ERR:
      return "Error";
    case LVAL_FUN:
      return "Function";
    case LVAL_STR:
      return "String";
    case LIST:
      return "List";
    case PLIST:
      return "Plist";
    case VECTOR:
      return "Vector";
    case MAP:
      return "Map";
    default:
      return "Unknown";
  }
}

void lval_del(Lval* lval) {
  return;
  /* printf("freeing: "); */
  /* lval_println(lval); */
  switch (lval->type) {
    case LVAL_NUM:
      break;
    case LVAL_STR:
      free(lval->str);
      break;
    case LVAL_SYM:
      free(lval->sym);
      break;
    case LVAL_ERR:
      free(lval->err);
      break;
    case LVAL_SEQ:
      list_free(lval->cell);
      break;
    case LVAL_FUN:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        free(lval->func_name);
      } else {
        lenv_del(lval->bindings);
        lval_del(lval->formals);
        lval_del(lval->body);
      }
      break;
    case PLIST:
      printf("how to delete a plist??\n");
      list_free(lval->cell);
      /* just free the point pointer I'd say, but then do reference counting to
       * free any cdr */
      break;
  }
  free(lval);
}

Lval* make_lval_copy(Lval* lval) {
  if (lval->type == LVAL_SEQ && lval->subtype == PLIST) return lval;
  /* printf("make_lval_copy\n"); */
  Lval* x = lalloc(LVAL);
  /* printf("make_lval_copy\n"); */
  /* lval_println(lval); */
  x->type = lval->type;
  x->subtype = lval->subtype;
  x->tco_env = lval->tco_env;
  /* printf("making copy, cdr: %p\n", lval->cdr); */
  /* printf("switching\n"); */
  switch (lval->type) {
    case LVAL_FUN:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        x->fun = lval->fun;
        x->func_name = calloc(1, strlen(lval->func_name) + 1);
        strcpy(x->func_name, lval->func_name);
      } else {
        /* printf("in lval_copy\n"); */
        /* lval_println(lval); */
        x->bindings = lenv_copy(lval->bindings);
        /* printf("copied ENV\n"); */
        x->formals = make_lval_copy(lval->formals);
        /* printf("copied FORMALS\n"); */
        /* lval_println(lval->body); */
        /* printf("%s\n", lval_type_to_name(lval->body->type)); */
        x->body = make_lval_copy(lval->body);
        /* printf("copied BODY\n"); */
      }
      break;
    case LVAL_NUM:
      x->num = lval->num;
      break;

    case LVAL_ERR:
      x->err = calloc(1, strlen(lval->err) + 1);
      strcpy(x->err, lval->err);
      break;
    case LVAL_STR:
      x->str = calloc(1, strlen(lval->str) + 1);
      strcpy(x->str, lval->str);
      break;
    case LVAL_SYM:
      x->sym = calloc(1, strlen(lval->sym) + 1);
      strcpy(x->sym, lval->sym);
      break;
    case LVAL_SEQ:
      /* printf("copying seq\n"); */
      x->count = lval->count;
      x->node = calloc(1, sizeof(Lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->node[i] = make_lval_copy(lval->node[i]);
      }
      break;
    default:
      printf("Don't know how to print %s\n", lval_type_to_name2(lval));
  }
  return x;
}

Lval* lval_add_child(Lval* lval, Lval* x) {
  lval->count++;
  lval->node = realloc(lval->node, sizeof(Lval*) * lval->count);
  lval->node[lval->count - 1] = x;
  return lval;
}

Lval* lval_pop(Lval* lval, int i) {
  Lval* x = lval->node[i];
  memmove(&lval->node[i], &lval->node[i + 1],
          sizeof(Lval*) * (lval->count - i - 1));
  lval->count--;
  lval->node = realloc(lval->node, sizeof(Lval*) * lval->count);
  return x;
}

Lval* lval_take(Lval* v, int i) {
  Lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

Lval* lval_concat(Lval* x, Lval* y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add_child(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

Lval* make_lval_plist() {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_SEQ;
  lval->subtype = PLIST;
  lval->tco_env = NULL;
  lval->cell = NULL;
  return lval;
}
