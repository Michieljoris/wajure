#include "lval.h"

#include <stddef.h>

#include "cell.h"
#include "ltypes.h"
#include "namespace.h"

/* #include "hash.h" */
#include "env.h"
#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"

#ifdef WASM
#define INIT(FN_TABLE_INDEX) .fn_table_index = FN_TABLE_INDEX
#else
#define INIT(type) .ns = get_current_ns()
#endif

/* SYMBOL */

int seed = 123;
int str_seed = 124;
int sym_seed = 125;

int lval_hash(Lval* lval) {
  int hash = -1;
  switch (lval->group) {
    case LVAL_FUNCTION:
    case LVAL_ERR:
      /* hash = murmur3_int(long_hash((unsigned long)lval), seed); */
      break;
    case LVAL_SYMBOL:
      /* hash = murmur3_str(lval->str, _strlen(lval->str), sym_seed); */
      break;
    case LVAL_LITERAL:
      switch (lval->type) {
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
          printf("OOPS, can't calculate hash of unknown collection type %d\n",
                 lval->type);
      }
      break;

    case LVAL_COLLECTION:
      switch (lval->type) {
        case LIST:
        case VECTOR:
          hash = 3;
          break;
        case MAP:
          hash = 4;
          break;
        default:
          printf("OOPS, can't calculate hash of unknown collection type %d\n",
                 lval->type);
      }
      break;
    /* case LVAL_NAMESPACE: */
    /*   break; */
    default:
      printf("OOPS, can't calculate hash of unknown type %d\n", lval->group);
  }
  return hash;
}

Lval* make_lval_sym(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_SYMBOL,
                 .type = -1,
                 .data.str = retain(s),
                 INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_keyword(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_LITERAL,
                 .type = KEYWORD,
                 .data.str = retain(s),
                 INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

/* COLLECTION */

Lval* make_lval_list(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval =
      (Lval){.group = LVAL_COLLECTION, .type = LIST, INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* new_lval_list(void* head) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_COLLECTION,
                 .type = LIST,
                 .data.head = head,
                 INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* new_lval_vector(void* head) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_COLLECTION,
                 .type = VECTOR,
                 .data.head = head,
                 INIT(FTI_VECTOR)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_vector(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_COLLECTION, .type = VECTOR, INIT(FTI_MAP)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_map(void) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_COLLECTION, .type = MAP, INIT(FTI_MAP)};
  lval->hash = lval_hash(lval);
  return lval;
}

/* LITERAL */

Lval* make_lval_nil() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_LITERAL, .type = LNIL, INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_true() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_LITERAL, .type = LTRUE, INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_false() {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_LITERAL, .type = LFALSE, INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_num(long x) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_LITERAL,
                 .type = NUMBER,
                 .data.num = x,
                 INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

Lval* make_lval_str(char* s) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_LITERAL,
                 .type = STRING,
                 .data.str = retain(s),
                 INIT(FTI_RTE_NOT_A_FN)};
  lval->hash = lval_hash(lval);
  return lval;
}

/* FUNCTION */

// SYSTEM and SPECIAL
Lval* make_lval_fun(Lbuiltin func, char* func_name, int type) {
#ifdef WASM
  return NULL;
#else
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_FUNCTION,
                 .type = type,
                 .c_fn = func,
                 .data.str = retain(func_name),
                 .fn_table_index = -1,
                 INIT(-1)};
  lval->hash = lval_hash(lval);
  return lval;
#endif
}

Lambda* make_lambda(Lval* params, int param_count, int has_rest_arg,
                    Lval* body) {
  Lambda* lambda = lalloc_type(FN);
  *lambda = (Lambda){
      .params = params,
      .param_count = param_count,
      .has_rest_arg = has_rest_arg,
      .body = body,
  };
  return lambda;
}

Lambda* make_lambda_err(Lval* error) {
  Lambda* lambda = lalloc_type(FN);
  *lambda = (Lambda){.err = error};
  return lambda;
}

// LAMBDA and MACRO
Lval* make_lval_lambda(Lenv* env, int type, Lambda** lambdas) {
#ifdef WASM
  return NULL;
#else
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_FUNCTION,
                 .type = type,
                 .closure = env,
                 .lambdas = lambdas,
                 .fn_table_index = -1,
                 INIT(-1)};
  lval->hash = lval_hash(lval);
  return lval;
#endif
}

/* ERROR */

// System error
Lval* make_lval_err(char* fmt, ...) {
  Lval* lval = lalloc_type(LVAL);
  *lval = (Lval){.group = LVAL_ERR,
                 .type = SYS,
                 .data.str = lalloc_size(512),
                 INIT(FTI_RTE_NOT_A_FN)};

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
  lval->type = USER;
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
      return "string";
    case LIST:
      return "list";
    case VECTOR:
      return "vector";
    case MAP:
      return "map";
    case SET:
      return "set";

    case KEYWORD:
      return "keyword";

    case LTRUE:
      return "true";
    case LFALSE:
      return "false";
    case LNIL:
      return "nil";
    case LVAL_REF:
      return "Local ref";
    case REGEX:
      return "regex";
    case SYS:
      return "sys";
    case LAMBDA:
      return "lambda";
    case MACRO:
      return "macro";
    case SPECIAL:
      return "special";
    case LVAL_EXTERNAL:
      return "External";
    case PARAM:
      return "param";
    case LOCAL:
      return "local";
    default:
      return "Unknown";
  }
}

char* lval_type_to_name(Lval* lval) {
  switch (lval->group) {
    case LVAL_SYMBOL:
      return "Symbol";
    case LVAL_COLLECTION:
      switch (lval->type) {
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
      switch (lval->type) {
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
      switch (lval->type) {
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
