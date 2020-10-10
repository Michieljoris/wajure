#include "lval.h"

#include <stddef.h>

#include "cell.h"

/* #include "hash.h" */
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"

/* SYMBOL */

int seed = 123;
int str_seed = 124;
int sym_seed = 125;

int lval_hash(Lval* lval) {
  int hash = -1;
  switch (lval->type) {
    case LVAL_FUNCTION:
    case LVAL_ERR:
      /* hash = murmur3_int(long_hash((unsigned long)lval), seed); */
      break;
    case LVAL_SYMBOL:
      /* hash = murmur3_str(lval->str, _strlen(lval->str), sym_seed); */
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case STRING:
          /* hash = murmur3_str(lval->str, _strlen(lval->str), str_seed);  // */
          break;
        case KEYWORD:
          /* hash = murmur3_str(lval->str, _strlen(lval->str), sym_seed); */
          break;
        case NUMBER:
          /* hash = murmur3_int(long_hash_munge(lval->num), seed); */
          break;
        case LNIL:
          hash = 0;
          break;
        case LTRUE:
          hash = 1;
          break;
        case LFALSE:
          hash = 2;
          break;
        default:
          printf(
              "OOPS, can't calculate hash of unknown collection subtype %d\n",
              lval->subtype);
      }
      break;

    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
        case VECTOR:
          hash = 3;
          break;
        case MAP:
          hash = 4;
          break;
        default:
          printf(
              "OOPS, can't calculate hash of unknown collection subtype %d\n",
              lval->subtype);
      }
      break;
    /* case LVAL_NAMESPACE: */
    /*   break; */
    default:
      printf("OOPS, can't calculate hash of unknown type %d\n", lval->type);
  }
  return hash;
}

Lval* make_lval_sym(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_SYMBOL, .subtype = -1, .data.str = retain(s)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_keyword(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval =
      (Lval){.type = LVAL_LITERAL, .subtype = KEYWORD, .data.str = retain(s)};
  lval->hash = lval_hash(lval);
  return lval;
}

/* COLLECTION */

Lval* make_lval_list(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = LIST};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* new_lval_list(void* head) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = LIST, .head = head};
  /* printf("arg list to cons (new_lval_list):\n"); */
  /* printf("lval_list: %d\n", get_ref_count(lval)); */
  /* printf("lval_list->head: %d\n", get_ref_count(lval->head)); */
  /* printf("lval_list->head->car: %d\n", get_ref_count(lval->head->car)); */
  /* printf("lval_list->head->cdr: %d\n", get_ref_count(lval->head->cdr)); */
  /* printf("lval_list->head->cdr-car: %d\n",
   * get_ref_count(lval->head->cdr->car)); */
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* new_lval_vector(void* head) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = VECTOR, .head = head};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_vector(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = VECTOR};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_map(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_COLLECTION, .subtype = MAP};
  lval->hash = lval_hash(lval);
  return lval;
}

/* LITERAL */

Lval* make_lval_nil() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = LNIL};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_true() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = LTRUE};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_false() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = LFALSE};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_num(long x) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_LITERAL, .subtype = NUMBER, .data.num = x};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_str(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){
      .type = LVAL_LITERAL,
      .subtype = STRING,
      .data.str = retain(s),
  };
  lval->hash = lval_hash(lval);
  return lval;
}

/* FUNCTION */

// SYSTEM and SPECIAL
Lval* make_lval_fun(Lbuiltin func, char* func_name, int subtype) {
#ifdef WASM
  return NULL;
#else
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_FUNCTION,
                 .subtype = subtype,
                 .fun = func,
                 .data.str = retain(func_name),
                 .offset = -1};
  lval->hash = lval_hash(lval);
  return lval;
#endif
}

// LAMBDA and MACRO
Lval* make_lval_lambda(Lenv* env, Lval* params, Lval* body, int subtype) {
#ifdef WASM
  return NULL;
#else
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.type = LVAL_FUNCTION,
                 .subtype = subtype,
                 .closure = env,
                 .params = params,
                 .body = body,
                 .offset = -1};
  lval->hash = lval_hash(lval);
  return lval;
#endif
}

/* ERROR */

// System error
Lval* make_lval_err(char* fmt, ...) {
  Lval* lval = lalloc_type(LVAL);
  *lval =
      (Lval){.type = LVAL_ERR, .subtype = SYS, .data.str = lalloc_size(512)};

  va_list va;
  va_start(va, fmt);
  vsnprintf(lval->data.str, 511, fmt, va);
  lval->data.str = lrealloc(lval->data.str, _strlen(lval->data.str) + 1);
  va_end(va);
  lval->hash = lval_hash(lval);
  return lval;
}

// User error
Lval* make_lval_exception(char* msg) {
  Lval* lval = make_lval_err(msg);
  lval->subtype = USER;
  return lval;
}

// Compiler type
/* Lval* make_lval_external(struct resolved_symbol s) { */
/*   Lval* lval = lalloc_type(LVAL); */

/*   *lval = (Lval){ */
/*       .type = LVAL_EXTERNAL, .subtype = s.lval->subtype, .str =
 * retain(s.fqn)}; */
/*   return lval; */
/* } */

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
    /* case LVAL_NAMESPACE: */
    /*   return "Namespace"; */
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

    case KEYWORD:
      return "Keyword";

    case LTRUE:
      return "true";
    case LFALSE:
      return "false";
    case LNIL:
      return "nil";
    case LVAL_REF:
      return "Local ref";
    case WVAL_FUN:
      return "lval wasm lambda";
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
          return "List";
        case VECTOR:
          return "Vector";
        case MAP:
          return "Map";
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
        case KEYWORD:
          return "Keyword";
      }
    case LVAL_FUNCTION:
      switch (lval->subtype) {
        case SYS:
          return "Function (sys)";
        case LAMBDA:
          return "Function";
        case SPECIAL:
          return "Special form";
        case MACRO:
          return "Macro";
      }
    /* case LVAL_NAMESPACE: */
    /*   return "Namespace"; */
    case LVAL_ERR:
      return "Error";
    default:
      return "Unknown";
  }
}

char* get_namespace_part(Lval* lval_sym) {
  char* str = lval_sym->data.str;
  char* pos = _strchr(str, '/');
  if (pos > str && pos < str + _strlen(str) - 1) {
    int namespace_len = pos - str;
    char* namespace = lalloc_size(namespace_len + 1);
    _strncpy(namespace, str, namespace_len);
    namespace[namespace_len] = '\0';
    return namespace;
  }
  return NULL;
}

char* get_name_part(Lval* lval_sym) {
  char* str = lval_sym->data.str;
  char* pos = _strchr(str, '/');
  if (pos > str && pos < str + _strlen(str) - 1) {
    pos++;
    int name_len = _strlen(str) - (pos - str);
    char* name = lalloc_size(name_len + 1);
    _strcpy(name, pos);
    return name;
  }
  return retain(lval_sym->data.str);
}
