
#include "special.h"

#include <stdio.h>
#include <string.h>

#include "lval.h"

typedef struct {
  char* key;
  int val;
} t_symstruct;

enum { QUOTE, DEFMACRO };
static t_symstruct special_syms[] = {{"quote", QUOTE}, {"defmacro", DEFMACRO}};
long special_syms_count = sizeof(special_syms) / sizeof(t_symstruct);

int lookup_special_sym(char* sym) {
  for (int i = 0; i < special_syms_count; i++) {
    if (strcmp(special_syms[i].key, sym) == 0) {
      return special_syms[i].val;
    }
  }
  return -1;
}

Lval* eval_special(Lenv* env, long special_sym, Lval* sexpr_args) {
  switch (special_sym) {
    case QUOTE:
      printf("QUOTE\n");
      return sexpr_args;
      break;
    case DEFMACRO:
      printf("DEFMACRO: not implemented yet!!\n");
      break;
  }
  return sexpr_args;
}
