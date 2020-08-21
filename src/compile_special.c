#include "compile_special.h"

#include <binaryen-c.h>

#include "assert.h"
#include "compile.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "print.h"
#include "wasm.h"
#include "wasm_util.h"

Ber compile_let(Wasm* wasm, Cell* arg_list) {
  BinaryenModuleRef module = wasm->module;
  CONTEXT_CELL("compile_let", arg_list)
  /* printf("compile let call!!!!!!!!!!!!!!!\n"); */
  /* Ber let_body[] = {make_int32(module, 123)}; */

  if (list_count(arg_list) == 0)
    quit(wasm, "Error: let needs at least binding vector");

  Lval* bindings = arg_list->car;

  if (bindings->subtype != VECTOR)
    quit(wasm, "Error: Expected vector as first arg to let, got %s ",
         lval_type_to_name(bindings));

  int bindings_count = list_count(bindings->head);
  if (bindings_count % 2 != 0)
    quit(wasm, "Error: Binding vector for let has odd number of forms");

  bindings_count = bindings_count / 2;

  Lenv* let_env = enter_env(wasm);
  int let_body_count = list_count(arg_list) - 1;
  /* printf("let body count: %d:", let_body_count); */
  let_body_count = let_body_count == 0 ? 1 : let_body_count;
  let_body_count += bindings_count;
  Ber* let_body = malloc(let_body_count * sizeof(Ber));  // TODO: free???

  scoped_iter Cell* b = iter_new(bindings);
  Lval* lval_sym = iter_next(b);

  // Compile

  // Bindings
  int i = 0;
  while (lval_sym) {
    if (lval_sym->type != LVAL_SYMBOL) {
      quit(wasm, "Canot bind non-symbol. Got %s, expected %s.",
           lval_type_to_name(lval_sym),
           lval_type_constant_to_name(LVAL_SYMBOL));
    }

    Lval* lval = iter_next(b);
    Ber wasm_value = lval_compile(wasm, lval);
    Ber local_var = BinaryenLocalSet(
        module, context->function_context->local_count, wasm_value);
    let_body[i] = local_var;

    // LENV_PUT
    int local_index = context->function_context->local_count++;
    lenv_put(let_env, retain(lval_sym),
             make_lval_compiler(context, LOCAL, local_index));

    i++;
    /* if (lval->type == LVAL_ERR) { */
    /*   release(let_env); */
    /*   return lval; */
    /* } */

    lval_sym = iter_next(b);
  }

  // Body
  if (arg_list->cdr) {
    while (arg_list->cdr) {
      arg_list = arg_list->cdr;
      Ber ber = lval_compile(wasm, arg_list->car);
      if (arg_list->cdr) ber = BinaryenDrop(module, ber);
      let_body[i] = ber;
      i++;
    };
  } else {
    if (!wasm->lval_nil_offset)
      wasm->lval_nil_offset = make_lval_literal(wasm, make_lval_nil());
    let_body[i] = wasm->lval_nil_offset;
  }

  // Post compile
  leave_env(wasm);

  return BinaryenBlock(module, "let", let_body, let_body_count,
                       make_type_int32(1));
}

Ber compile_if(Wasm* wasm, Cell* args) {
  printf("Compiling if!!!!\n");
  return make_int32(wasm->module, 0);
}

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
