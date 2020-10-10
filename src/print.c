#include "print.h"

#include <stddef.h>

#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "platform.h"
#include "printf.h"
#include "runtime.h"

struct s_info;

static char* mpcf_escape_new(char* x, const char* input, const char** output) {
  int i;
  int found;
  char buff[2];
  char* s = x;
  char* y = lalloc_size(1);

  while (*s) {
    i = 0;
    found = 0;

    while (output[i]) {
      if (*s == input[i]) {
        y = lrealloc(y, _strlen(y) + _strlen(output[i]) + 1);
        _strcat(y, output[i]);
        found = 1;
        break;
      }
      i++;
    }

    if (!found) {
      y = lrealloc(y, _strlen(y) + 2);
      buff[0] = *s;
      buff[1] = '\0';
      _strcat(y, buff);
    }

    s++;
  }

  return y;
}

static const char mpc_escape_input_c[] = {'\a', '\b', '\f', '\n', '\r', '\t',
                                          '\v', '\\', '\'', '\"', '\0'};

static const char* mpc_escape_output_c[] = {"\\a", "\\b",  "\\f", "\\n",
                                            "\\r", "\\t",  "\\v", "\\\\",
                                            "\\'", "\\\"", "\\0", NIL};

/* char* mpcf_escape(char* x) { */
/*   char* y = mpcf_escape_new(x, mpc_escape_input_c, mpc_escape_output_c); */
/*   free(x); */
/*   return y; */
/* } */

int lval_print_str(void (*out)(char character, void* arg), void* arg,
                   Lval* lval) {
  /* char* escaped = lalloc_size(_strlen(lval->data.str) + 1); */
  /* _strcpy(escaped, lval->data.str); */
  /* char* to_escape = retain(lval->data.str); */
  /* escaped = mpcf_escape(escaped); */
  scoped char* escaped =
      mpcf_escape_new(lval->data.str, mpc_escape_input_c, mpc_escape_output_c);
  return fctprintf(out, arg, "\"%s\"", escaped);
}

int lval_pr_str(void (*out)(char character, void* arg), void* arg, Lval* lval) {
  /* scoped char* escaped = */
  /*     mpcf_escape_new(lval->data.str, mpc_escape_input_c,
   * mpc_escape_output_c); */
  /* return fctprintf(out, arg, "%s", escaped); */
  return fctprintf(out, arg, "%s", lval->data.str);
  /* char* escaped = lalloc_size(_strlen(lval->data.str) + 1); */
  /* _strcpy(escaped, lval->data.str); */
  /* escaped = mpcf_escape(escaped); */
  /* printf("%s", escaped); */
  /* release(escaped); */
}

int _lval_print(void (*out)(char character, void* arg), void* arg, Lval* lval);

int lval_collection_print(void (*out)(char character, void* arg), void* arg,
                          Lval* lval, char open, char close) {
  /* printf("Printing collectin:\n"); */
  int ret = 0;
  if (open) {
    out(open, arg);
    ret++;
  }
  Cell* cell = lval->head;
  while (cell) {
    ret += _lval_print(out, arg, cell->car);
    cell = cell->cdr;
    if (cell) {
      out(' ', arg);
      ret++;
    }
  }
  if (close) {
    out(close, arg);
    ret++;
  }
  return ret;
}

int lval_fun_print(void (*out)(char character, void* arg), void* arg,
                   Lval* lval) {
  int ret = 0;
  switch (lval->subtype) {
    case SYS:
      ret = fctprintf(out, arg, "<function %s>", lval->data.str);
      break;
    case LAMBDA:
    case MACRO:;
#ifdef WASM
#else
      char* fn_name = lval->subtype == LAMBDA ? "fn" : "macro";
      ret += fctprintf(out, arg, "(%s ", fn_name);
      ret += _lval_print(out, arg, lval->params);
      out(' ', arg);
      ret++;
      ret += lval_collection_print(out, arg, lval->body, 0, 0);
      out(')', arg);
      ret++;
#endif
      break;
    case SPECIAL:
      ret = fctprintf(out, arg, "<function %s>", lval->data.str);
      break;
  }
  return ret;
}

int _lval_print(void (*out)(char character, void* arg), void* arg, Lval* lval) {
  if (!lval) return fctprintf(out, arg, "<Trying to print null lval>");
  /* printf("in lval print %d %s\n", lval->type, */
  /*        lval_type_constant_to_name(lval->type)); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      return fctprintf(out, arg, "%s", lval->data.str);
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          return lval_collection_print(out, arg, lval, '(', ')');
        case MAP:
          return lval_collection_print(out, arg, lval, '{', '}');
        case VECTOR:
          return lval_collection_print(out, arg, lval, '[', ']');
        default:
          return fctprintf(out, arg, "unknown lval subtype %s\n",
                           lval_type_constant_to_name(lval->subtype));
      }
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          return fctprintf(out, arg, "%li", lval->data.num);
        case STRING:
          return lval_print_str(out, arg, lval);
        case KEYWORD:
          return fctprintf(out, arg, ":%s", lval->data.str);
        case LNIL:
          return fctprintf(out, arg, "nil");
        case LTRUE:
          return fctprintf(out, arg, "true");
        case LFALSE:
          return fctprintf(out, arg, "false");
      }
    case LVAL_FUNCTION:
      return lval_fun_print(out, arg, lval);
#ifdef WASM
#else
    case LVAL_REF:
      switch (lval->subtype) {
        case PARAM:
          return fctprintf(out, arg, "P%d", lval->offset);
        case LOCAL:
          return fctprintf(out, arg, "L%d", lval->offset);
      }
#endif
    case LVAL_ERR:
      return fctprintf(out, arg, "Error: %s", lval->data.str);
    default:
      return fctprintf(out, arg, "unknown lval type %d, %s\n", lval->type,
                       lval_type_constant_to_name(lval->type));
  }
}

void print_kv(void* pair) {
  lval_print((Lval*)((Cell*)pair)->car);
  printf(": ");
  lval_print((Lval*)((Cell*)pair)->cdr);
}

void alist_print(Cell* alist) { list_print(alist, print_kv, "\n"); }

// Debug version
void lenv_print(Lenv* env) {
  if (log_level < LOG_LEVEL_DEBUG) return;
  if (env->parent_env) {
    alist_print(env->kv);
  } else {
    printf("ROOT env!!! \n");
    /* list_print(env->kv, print_kv, "\n"); */
  }
}

void env_print(Lenv* env) {
  if (env->parent_env) {
    alist_print(env->kv);
  } else {
    /* printf("ROOT env!!! \n"); */
    list_print(env->kv, print_kv, "\n");
  }
}

void out(char character, void* arg) { putchar(character); }

void lval_print(Lval* lval) {
  void* arg = NULL;
  _lval_print(&out, arg, lval);
}

// TODO: this one prints without quotes. Make a proper pprint fn, and make this
// the normal print (so without quotes)
void lval_pr(Lval* lval) {
  void* arg = NULL;
  if (lval->subtype == STRING) {
    lval_pr_str(out, arg, lval);
    return;
  }
  _lval_print(&out, arg, lval);
}

struct s_info {
  char* str;
  int maxlen;
  int* index;
};

void s_out(char character, void* arg) {
  struct s_info* d = ((struct s_info*)arg);
  char* buf = d->str;
  int maxlen = d->maxlen;
  int* index = d->index;
  if (*index < maxlen - 1) {
    buf[*index] = character;
    (*index)++;
  } else if (*index == maxlen - 1) {
    /* printf("WARNING: can't build a string longer than %d chars\n", maxlen -
     * 1); */
    buf[*index] = '\0';
    (*index)++;
  } else {
    (*index)++;
  }
}

int lval_snprint(char* str, int maxlen, Lval* lval) {
  int* index = lalloc_size(sizeof(int*));
  *index = 0;
  struct s_info arg = {str, maxlen, index};
  if (lval->subtype == STRING)
    lval_pr_str(s_out, &arg, lval);
  else
    _lval_print(&s_out, &arg, lval);
  int len = *index;
  if (*index < maxlen - 1) {
    str[*index] = '\0';
  }
  s_out('\0', &arg);
  release(index);
  return len;
}

void lval_info(Lval* lval) {
  if (log_level < LOG_LEVEL_INFO) return;
  lval_print(lval);
}

void lval_debug(Lval* lval) {
  if (log_level < LOG_LEVEL_DEBUG) return;
  lval_print(lval);
}

void lval_println(Lval* v) {
  /* printf("type, %d, subtype %d, num %li\n ", v->type, v->subtype, v->num); */
  lval_print(v);
  putchar('\n');
}

void lval_debugln(Lval* v) {
  if (log_level < LOG_LEVEL_DEBUG) return;
  lval_print(v);
  putchar('\n');
}

void lval_infoln(Lval* v) {
  if (log_level < LOG_LEVEL_INFO) return;
  lval_print(v);
  putchar('\n');
}
