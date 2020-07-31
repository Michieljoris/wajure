#include "compile_special.h"

#include <binaryen-c.h>

#include "assert.h"
#include "list.h"
#include "print.h"
#include "wasm.h"
#include "wasm_util.h"

Ber compile_let(Wasm* wasm, Cell* arg_list) {
  NEW_CONTEXT_CELL("compile_let", arg_list)
  printf("compile let call!!!!!!!!!!!!!!!\n");
  BinaryenModuleRef module = wasm->module;
  Ber let_body[] = {make_int32(module, 123)};
  int let_body_count = 1;

  if (list_count(arg_list) == 0)
    quit(wasm, "Error: let needs at least binding vector");

  Lval* binding_vector = arg_list->car;

  if (binding_vector->subtype != VECTOR)
    quit(wasm, "Error: Expected vector as first arg to let, got %s ",
         lval_type_to_name(binding_vector));

  return BinaryenBlock(module, "let", let_body, let_body_count,
                       make_type_int32(1));
}

Ber compile_if(Wasm* wasm, Cell* args) { return make_int32(wasm->module, 0); }

Ber compile_try(Wasm* wasm, Cell* args) { return make_int32(wasm->module, 0); }

Ber compile_throw(Wasm* wasm, Cell* args) {
  return make_int32(wasm->module, 0);
}
Ber compile_quote(Wasm* wasm, Cell* args) {
  return make_int32(wasm->module, 0);
}
Ber compile_quasiquote(Wasm* wasm, Cell* args) {
  return make_int32(wasm->module, 0);
}

/* LASSERT(arg_list, list_count(arg_list->head) >= 1, */
/*         "Error: let needs at least binding vector") */

/* scoped_iter Cell* a = iter_new(arg_list); */
/* Lval* bindings = iter_next(a); */
/* LASSERT_TYPE("let", arg_list, 0, LVAL_COLLECTION, VECTOR, bindings); */
/* LASSERT(arg_list, list_count(bindings->head) % 2 == 0, */
/*         "Binding vector for let has odd number of forms"); */

/* Lenv* let_env = lenv_new(); */
/* let_env->parent_env = retain(env); */

/* scoped_iter Cell* b = iter_new(bindings); */
/* Lval* lval_sym = iter_next(b); */

/* while (lval_sym) { */
/*   if (lval_sym->type != LVAL_SYMBOL) { */
/*     release(let_env); */
/*     return make_lval_err("Canot bind non-symbol. Got %s, expected %s.", */
/*                          lval_type_to_name(lval_sym), */
/*                          lval_type_constant_to_name(LVAL_SYMBOL)); */
/*   } */

/*   Lval* lval = iter_next(b); */
/*   lval = lval_eval(let_env, lval); */
/*   if (lval->type == LVAL_ERR) { */
/*     release(let_env); */
/*     return lval; */
/*   } */
/*   Lenv* new_let_env = lenv_prepend(let_env, retain(lval_sym), lval); */
/*   new_let_env->parent_env = let_env; */

/*   let_env = new_let_env; */
/*   lval_sym = iter_next(b); */
/* } */

/* scoped Lval* body = make_lval_list(); */
/* body->head = retain(iter_cell(a)); */
/* Lval* ret = do_list(let_env, body, RETURN_ON_ERROR); */
/* release(let_env); */
/* return ret; */
