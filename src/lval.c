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
  lval->head = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_vector(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_COLLECTION;
  lval->subtype = VECTOR;
  lval->head = NULL;
  lval->tco_env = NULL;
  return lval;
}

Lval* make_lval_map(void) {
  Lval* lval = lalloc(LVAL);
  lval->type = LVAL_COLLECTION;
  lval->subtype = MAP;
  lval->head = NULL;
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
  lval->bindings = env;
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

char* lval_type_constant_to_name(int t) {
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
    case VECTOR:
      return "Vector";
    case MAP:
      return "Map";
    default:
      return "Unknown";
  }
}

char* lval_type_to_name(Lval* lval) {
  switch (lval->type) {
    case LVAL_SYMBOL:
      return "Symbol";
    case LVAL_COLLECTION:
      switch (lval->subtype) {
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

void lval_del(Lval* lval) {
  return;
  /* printf("freeing: "); */
  /* lval_println(lval); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      free(lval->sym);
      break;
    case LVAL_COLLECTION:
      list_free(lval->head);
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

Cell* iter_new(Lval* lval_list) {
  Cell* iterator = make_cell();
  iterator->car = lval_list->head;
  return iterator;
}

Cell* iter_cell(Cell* iterator) {
  if (!iterator->car) return NIL;
  return iterator->car;
}

Lval* iter_next(Cell* iterator) {
  if (!iterator->car) return NIL;
  Cell* p = iterator->car;
  Lval* next_lval = p->car;
  iterator->car = p->cdr;
  return next_lval;
}

Lval* iter_peek(Cell* iterator) {
  if (!iterator->car) return NIL;
  Cell* p = iterator->car;
  Lval* next_lval = p->car;
  return next_lval;
}

void iter_end(Cell* iterator) { lfree(CELL, iterator); }
