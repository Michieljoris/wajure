#include "env.h"
#include "list_fns.h"
#include "math_fns.h"
#include "misc_fns.h"
#include "special_fns.h"

void lenv_add_builtin_fns(Lenv* env) {
  lenv_add_math_fns(env);
  lenv_add_list_fns(env);
  lenv_add_misc_fns(env);
  lenv_add_special_fns(env);
}
