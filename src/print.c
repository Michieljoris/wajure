#include "print.h"

#include "io.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "platform.h"

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

void lval_print_str(Lval* lval) {
  /* char* escaped = lalloc_size(_strlen(lval->str) + 1); */
  /* _strcpy(escaped, lval->str); */
  /* char* to_escape = retain(lval->str); */
  /* escaped = mpcf_escape(escaped); */
  char* escaped =
      mpcf_escape_new(lval->str, mpc_escape_input_c, mpc_escape_output_c);
  printf("\"%s\"", escaped);
  release(escaped);
}

void lval_pr_str(Lval* lval) {
  char* escaped =
      mpcf_escape_new(lval->str, mpc_escape_input_c, mpc_escape_output_c);
  printf("%s", escaped);
  release(escaped);
  /* char* escaped = lalloc_size(_strlen(lval->str) + 1); */
  /* _strcpy(escaped, lval->str); */
  /* escaped = mpcf_escape(escaped); */
  /* printf("%s", escaped); */
  /* release(escaped); */
}

void lval_collection_print(Lval* lval, char open, char close) {
  if (open) putchar(open);
  Cell* cell = lval->head;
  while (cell) {
    lval_print(cell->car);
    cell = cell->cdr;
    if (cell) putchar(' ');
  }
  if (close) putchar(close);
}

void lval_fun_print(Lval* lval) {
  switch (lval->subtype) {
    case SYS:
      printf("<function %s>", lval->str);
      break;
    case LAMBDA:
    case MACRO:;
      char* fn_name = lval->subtype == LAMBDA ? "fn" : "macro";
      printf("(%s ", fn_name);
      lval_print(lval->params);
      putchar(' ');
      lval_collection_print(lval->body, 0, 0);
      putchar(')');
      break;
    case SPECIAL:
      printf("<function %s>", lval->str);
      break;
  }
}

void lval_print(Lval* lval) {
  if (!lval) {
    printf(
        "!! NULL LVAL !! NULL LVAL !! NULL LVAL !! NULL LVAL !! NULL LVAL !!");
    return;
  }
  /* printf("in lval print %s\n", lval_type_constant_to_name(lval)); */
  switch (lval->type) {
    case LVAL_SYMBOL:
      printf("%s", lval->str);
      break;
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          lval_collection_print(lval, '(', ')');
          break;
        case MAP:
          lval_collection_print(lval, '{', '}');
          break;
        case VECTOR:
          lval_collection_print(lval, '[', ']');
          break;
        default:
          printf("unknown lval subtype %s\n",
                 lval_type_constant_to_name(lval->subtype));
      }
      break;
    case LVAL_LITERAL:
      switch (lval->subtype) {
        case NUMBER:
          printf("%li", lval->num);
          break;
        case STRING:
          lval_print_str(lval);
          break;
        case LNIL:
          printf("nil");
          break;
        case LTRUE:
          printf("true");
          break;
        case LFALSE:
          printf("false");
          break;
      }
    case LVAL_FUNCTION:
      lval_fun_print(lval);
      break;
    case LVAL_ERR:
      printf("Error: %s", lval->str);
      break;
    default:
      printf("unknown lval type %d, %s\n", lval->type,
             lval_type_constant_to_name(lval->type));
  }
}

void lval_info(Lval* lval) {
  if (log_level < LOG_LEVEL_INFO) return;
  lval_print(lval);
}

void lval_debug(Lval* lval) {
  if (log_level < LOG_LEVEL_DEBUG) return;
  lval_print(lval);
}

// TODO: this one prints without quotes. Make a proper pprint fn, and make
// this the normal print (so without quotes)
void lval_pr(Lval* lval) {
  if (lval->subtype == STRING) {
    lval_pr_str(lval);
    return;
  }
  lval_info(lval);
}

void lval_println(Lval* v) {
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
