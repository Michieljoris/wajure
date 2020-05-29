
/* typedef struct { */
/*   char* key; */
/*   int val; */
/* } t_symstruct; */

/* enum { QUOTE, QUASIQUOTE, DEF, LAMBDA, IF, MACRO, MACROEXPAND }; */
/* static t_symstruct special_syms[] = {{"quote", QUOTE}, */
/*                                      {"quasiquote", QUASIQUOTE}, */
/*                                      {"def", DEF}, */
/*                                      {"fn", LAMBDA}, */
/*                                      {"if", IF}, */
/*                                      {"macro", MACRO}, */
/*                                      {"macroexpand", MACROEXPAND}}; */
/* long special_syms_count = sizeof(special_syms) / sizeof(t_symstruct); */

/* int lookup_special_sym(char* sym) { */
/*   for (int i = 0; i < special_syms_count; i++) { */
/*     if (strcmp(special_syms[i].key, sym) == 0) { */
/*       return special_syms[i].val; */
/*     } */
/*   } */
/*   return -1; */
/* } */

/* Lval* eval_special(Lenv* env, long special_sym, Lval* sexpr_args) { */
/*   switch (special_sym) { */
/*     case QUOTE: */
/*       LASSERT_NODE_COUNT(sexpr_args, 1, "quote"); */
/*       return lval_take(sexpr_args, 0); */
/*     case QUASIQUOTE: */
/*       return eval_quasiquote(env, sexpr_args); */
/*     case DEF: */
/*       return eval_def(env, sexpr_args); */
/*     case IF: */
/*       return eval_if(env, sexpr_args); */
/*     case LAMBDA: */
/*       return eval_lambda(env, sexpr_args); */
/*     case MACROEXPAND: /\* TODO isn't a special form right *\/ */
/*       return eval_macroexpand(env, sexpr_args); */
/*     case MACRO: */
/*       return eval_macro(env, sexpr_args); */
/*     default: */
/*       return make_lval_err("%li : not implemented yet!!\n", special_sym);
 */
/*   } */
/* } */

/* int special_sym = lookup_special_sym(lval->node[0]->sym); */
/* if (special_sym != -1) { */
/*   lval_del(lval_pop(lval, 0)); */
/*   return eval_special(env, special_sym, lval); */
/* } */

/* Lval* eval_fun(Lenv* env, Lval* sexpr) { */
/*   sexpr = eval_nodes(env, sexpr, sexpr->count); */
/*   /\* if error return error!!! *\/ */

/*   /\* first expr should have evalled to a fun*\/ */
/*   Lval* lval_fun = lval_pop(sexpr, 0); */
/*   if (lval_fun->type != LVAL_FUN && lval_fun->type != LVAL_MACRO) { */
/*     lval_del(lval_fun); */
/*     lval_del(sexpr); */
/*     return make_lval_err( */
/*         "sexpr starts with incorrect type. " */
/*         "Got %s, expected %s.", */
/*         lval_type_to_name(lval_fun->type), lval_type_to_name(LVAL_FUN)); */
/*   } */

/*   /\* sexpr has all elements now except for first (a LVAL_FUN) *\/ */
/*   Lval* lval = lval_call(env, lval_fun, sexpr); */
/*   lval_del(lval_fun); */
/*   return lval; */
/* } */
