#include "print.h"

#include "env.h"
#include "io.h"
#include "lib.h"
#include "lval.h"
#include "mpc.h"

void lval_print_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  printf("\"%s\"", escaped);
  free(escaped);
}

void lval_pr_str(Lval* lval) {
  char* escaped = malloc(strlen(lval->str) + 1);
  strcpy(escaped, lval->str);
  escaped = mpcf_escape(escaped);
  printf("%s", escaped);
  free(escaped);
}

void lval_collection_print(Lval* lval, char open, char close) {
  if (open) putchar(open);
  Cell* cell = lval->head;
  while (cell) {
    lval_debug(cell->car);
    cell = cell->cdr;
    if (cell) putchar(' ');
  }
  if (close) putchar(close);
}

void lval_fun_print(Lval* lval) {
  switch (lval->subtype) {
    case SYS:
      printf("<function %s>", lval->func_name);
      break;
    case LAMBDA:
    case MACRO:;
      char* fn_name = lval->subtype == LAMBDA ? "fn" : "macro";
      printf("(%s ", fn_name);
      lval_debug(lval->params);
      putchar(' ');
      lval_collection_print(lval->body, 0, 0);
      putchar(')');
      break;
    case SPECIAL:
      printf("<function %s>", lval->func_name);
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
      printf("%s", lval->sym);
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
      }
    case LVAL_FUNCTION:
      lval_fun_print(lval);
      break;
    case LVAL_ERR:
      printf("Error: %s", lval->err);
      break;
    default:
      printf("unknown lval type %d, %s\n", lval->type,
             lval_type_constant_to_name(lval->type));
  }
}

void lval_info(Lval* lval) {
  if (*log_level < LOG_LEVEL_INFO) return;
  lval_print(lval);
}

void lval_debug(Lval* lval) {
  if (*log_level < LOG_LEVEL_DEBUG) return;
  lval_print(lval);
}

// TODO: this one prints without quotes. Make a proper pprint fn, and make
// this the normal print (so without quotes)
void lval_pr(Lval* lval) {
  if (*log_level < LOG_LEVEL_DEBUG) return;
  if (lval->subtype == STRING) {
    lval_pr_str(lval);
    return;
  }
  lval_info(lval);
}

void lval_debugln(Lval* v) {
  if (*log_level < LOG_LEVEL_DEBUG) return;
  lval_print(v);
  putchar('\n');
}

void lval_infoln(Lval* v) {
  if (*log_level < LOG_LEVEL_INFO) return;
  lval_print(v);
  putchar('\n');
}

void print_kv(void* pair) {
  lval_debug((Lval*)((Cell*)pair)->car);
  printf(": ");
  lval_debug((Lval*)((Cell*)pair)->cdr);
}

void alist_print(Cell* alist) { list_print(alist, print_kv, "\n"); }

void lenv_print(Lenv* env) {
  if (*log_level < LOG_LEVEL_DEBUG) return;
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
    list_print(env->kv, print_kv, "\n");
  }
}
