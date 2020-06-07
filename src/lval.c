#include "lval.h"

#include "env.h"
#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"

/* SYMBOL */

Lval* make_lval_sym(char* s) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_SYMBOL;
  lval->sym = calloc(1, _strlen(s) + 1);
  _strcpy(lval->sym, s);
  lval->tco_env = NULL;
  return lval;
}

/* COLLECTION */

Lval* make_lval_list(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_COLLECTION;
  lval->subtype = LIST;
  lval->list = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_vector(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_COLLECTION;
  lval->subtype = VECTOR;
  lval->list = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_map(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_COLLECTION;
  lval->subtype = MAP;
  lval->list = NULL;
  lval->tco_env = NULL;
  return lval;
}

/* LITERAL */

Lval* make_lval_num(long x) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_LITERAL;
  lval->subtype = NUMBER;
  lval->num = x;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_str(char* s) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_LITERAL;
  lval->subtype = STRING;
  lval->str = calloc(1, _strlen(s) + 1);
  _strcpy(lval->str, s);
  lval->tco_env = NULL;
  return lval;
}

/* FUNCTION */

// SYSTEM and SPECIAL
Lval* make_lval_fun(lbuiltin func, char* func_name, int subtype) {
  Lval* lval = lalloc(LVAL);
  lval->func_name = calloc(1, _strlen(func_name) + 1);
  _strcpy(lval->func_name, func_name);
  lval->type = LVAL_FUNCTION;
  lval->subtype = subtype;
  lval->fun = func;
  lval->tco_env = NULL;
  return lval;
}

// LAMBDA and MACRO
Lval* make_lval_lambda(Lenv* env, Lval* params, Lval* body, int subtype) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_FUNCTION;
  lval->subtype = subtype;
  lval->bindings = lenv_new();
  lval->bindings->parent_env = env;
  lval->params = params;
  lval->body = body;
  lval->tco_env = NULL;
  return lval;
}

/* ERROR */

// System error
Lval* make_lval_err(char* fmt, ...) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_ERR;
  lval->subtype = SYS;
  va_list va;
  va_start(va, fmt);
  lval->err = calloc(1, 512);
  vsnprintf(lval->err, 511, fmt, va);
  lval->err = realloc(lval->err, _strlen(lval->err) + 1);

  va_end(va);
  lval->tco_env = NULL;
  return lval;
}

// User error
Lval* make_lval_exception(char* msg) {
  Lval* lval = make_lval_err(msg);
  lval->subtype = USER;
  lval->tco_env = NULL;
  return lval;
}

char* lval_type_to_name2(Lval* lval) {
  switch (lval->type) {
    case LVAL_SYMBOL:
      return "Symbol";
    case LVAL_COLLECTION:
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
          return "unknown collection subtype";
      }
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          return "Number";
        case STRING:
          return "String";
      }
    case LVAL_FUNCTION:
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
    case LVAL_ERR:
      return "Error";
    default:
      return "Unknown";
  }
}

char* lval_type_to_name(int t) {
  switch (t) {
    case LVAL_LITERAL:
      return "Literal";
    case NUMBER:
      return "Number";
    case LVAL_SYMBOL:
      return "Symbol";
    case LVAL_COLLECTION:
      return "Seq";
    case LVAL_ERR:
      return "Error";
    case LVAL_FUNCTION:
      return "Function";
    case STRING:
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
    case LVAL_SYMBOL:
      free(lval->sym);
      break;
    case LVAL_COLLECTION:
      list_free(lval->list);
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          break;
        case STRING:
          free(lval->str);
          break;
        default:
          printf("Can't delete unknown literal subtype: %d\n", lval->subtype);
      }
      break;
    case LVAL_FUNCTION:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        free(lval->func_name);
      } else {
        lenv_del(lval->bindings);
        lval_del(lval->params);
        lval_del(lval->body);
      }
      break;
    case LVAL_ERR:
      free(lval->err);
      break;
    default:
      printf("Can't delete unknown type: %d\n", lval->type);
  }
  free(lval);
}

Lval* make_lval_copy(Lval* lval) {
  if (lval->type == LVAL_COLLECTION && lval->subtype == PLIST) return lval;
  printf("make_lval_copy:\n");
  lval_println(lval);
  /* printf("lval type and subtye %d %d\n", lval->type, lval->subtype); */
  Lval* x = lalloc(LVAL);
  x->type = lval->type;
  x->subtype = lval->subtype;
  x->tco_env = lval->tco_env;
  switch (lval->type) {
    case LVAL_SYMBOL:
      x->sym = calloc(1, _strlen(lval->sym) + 1);
      _strcpy(x->sym, lval->sym);
      break;
    case LVAL_COLLECTION:
      /* printf("copying seq\n"); */
      x->count = lval->count;
      x->node = calloc(1, sizeof(Lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->node[i] = make_lval_copy(lval->node[i]);
      }
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          x->num = lval->num;
          break;
        case STRING:
          x->str = calloc(1, _strlen(lval->str) + 1);
          _strcpy(x->str, lval->str);
          break;
        default:
          return make_lval_err(
              "Don't know how to copy unknown literal subtype :%d\n",
              lval->subtype);
      }
      break;
    case LVAL_FUNCTION:
      if (lval->subtype == SYS || lval->subtype == SPECIAL) {
        x->fun = lval->fun;
        x->func_name = calloc(1, _strlen(lval->func_name) + 1);
        _strcpy(x->func_name, lval->func_name);
      } else {
        /* printf("in lval_copy\n"); */
        /* lval_println(lval); */
        x->bindings = lenv_copy(lval->bindings);
        /* printf("copied ENV\n"); */
        x->params = make_lval_copy(lval->params);
        /* printf("copied FORMALS\n"); */
        /* lval_println(lval->body); */
        /* printf("%s\n", lval_type_to_name(lval->body->type)); */
        x->body = make_lval_copy(lval->body);
        /* printf("copied BODY\n"); */
      }
      break;
    case LVAL_ERR:
      x->err = calloc(1, _strlen(lval->err) + 1);
      _strcpy(x->err, lval->err);
      break;
    default:
      printf("Don't know how to copy unknown lval type: %s\n",
             lval_type_to_name2(lval));
  }
  /* printf("returning x:"); */
  /* lval_println(x); */
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
  _memmove(&lval->node[i], &lval->node[i + 1],
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
  lval->type = LVAL_COLLECTION;
  lval->subtype = PLIST;
  lval->tco_env = NULL;
  lval->list = NULL;
  return lval;
}

Cell* iter_new(Lval* lval_list) {
  Cell* iterator = make_cell();
  iterator->car = lval_list->list;
  return iterator;
}

Lval* iter_next(Cell* iterator) {
  if (!iterator->car) return NIL;
  Cell* p = iterator->car;
  Lval* next_lval = p->car;
  iterator->car = p->cdr;
  return next_lval;
}

void iter_end(Cell* iterator) { lfree(CELL, iterator); }
