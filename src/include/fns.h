#ifndef __FNS_H_
#define __FNS_H_

void lenv_add_builtins(Lenv* e);
bool exit_repl;
Lval* eval_fn(Lenv* env, Lval* sexpr);
Lval* list_fn(Lenv* e, Lval* sexpr);
#endif  // __FNS_H_
