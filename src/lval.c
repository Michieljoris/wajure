#include "lval.h"

#include "cell.h"
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"

/* SYMBOL */

Lval* make_lval_sym(char* s) {
  Lval* lval = lalloc_type(LVAL);
  /* *lval = (Lval){ */
  /*     .type = LVAL_SYMBOL, .subtype = -1, .sym = lalloc_size(_strlen(s) +
   * 1)}; */

  *lval = (Lval){.type = LVAL_SYMBOL, .subtype = -1, .sym = retain(s)};
  /* _strcpy(lval->sym, s); */
  return lval;
}

/* COLLECTION */

Lval* make_lval_list(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = LIST};
  return lval;
}

Lval* make_lval_vector(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = VECTOR};
  return lval;
}

Lval* make_lval_map(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = MAP};
  return lval;
}

/* LITERAL */

Lval* make_lval_nil() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = LNIL};
  return lval;
}

Lval* make_lval_true() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = LTRUE};
  return lval;
}

Lval* make_lval_false() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = LFALSE};
  return lval;
}

Lval* make_lval_num(long x) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = NUMBER, .num = x};
  return lval;
}

Lval* make_lval_str(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL,
                 .subtype = STRING,
                 /* .str = lalloc_size(_strlen(s) + 1)}; */
                 .str = retain(s)};
  /* _strcpy(lval->str, s); */
  return lval;
}

/* FUNCTION */

// SYSTEM and SPECIAL
Lval* make_lval_fun(Lbuiltin func, char* func_name, int subtype) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_FUNCTION,
                 .subtype = subtype,
                 .fun = func,
                 /* .func_name = lalloc_size(_strlen(func_name) + 1)}; */
                 .func_name = retain(func_name)};

  /* _strcpy(lval->func_name, func_name); */
  return lval;
}

// LAMBDA and MACRO
Lval* make_lval_lambda(Lenv* env, Lval* params, Lval* body, int subtype) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_FUNCTION,
                 .subtype = subtype,
                 .closure_env = env,
                 .params = params,
                 .body = body};
  return lval;
}

/* ERROR */

// System error
Lval* make_lval_err(char* fmt, ...) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_ERR, .subtype = SYS, .err = lalloc_size(512)};
  va_list va;
  va_start(va, fmt);
  vsnprintf(lval->err, 511, fmt, va);
  lval->err = lrealloc(lval->err, _strlen(lval->err) + 1);
  va_end(va);
  return lval;
}

// User error
Lval* make_lval_exception(char* msg) {
  Lval* lval = make_lval_err(msg);
  lval->subtype = USER;
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
      return "Collection";
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

    case LTRUE:
      return "true";
    case LFALSE:
      return "false";
    case LNIL:
      return "nil";
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
          return "List (coll)";
        case VECTOR:
          return "Vector (coll)";
        case MAP:
          return "Map (coll)";
        default:
          return "unknown collection subtype";
      }
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          return "Number";
        case STRING:
          return "String";
        case LTRUE:
          return "true";
        case LFALSE:
          return "false";
        case LNIL:
          return "nil";
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
