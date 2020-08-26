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

CResult compile_let(Wasm* wasm, Cell* arg_list) {
  BinaryenModuleRef module = wasm->module;
  CONTEXT_CELL("compile_let", arg_list)
  /* printf("compile let call!!!!!!!!!!!!!!! %p\n", arg_list); */

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
  int let_body_count_max = list_count(arg_list);
  let_body_count_max += bindings_count;
  Ber* let_body = malloc(let_body_count_max * sizeof(Ber));

  scoped_iter Cell* b = iter_new(bindings);
  Lval* lval_sym = iter_next(b);

  // Compile

  LocalIndices* li = li_init();

  // Bindings
  int let_body_count = 0;
  while (lval_sym) {
    if (lval_sym->type != LVAL_SYMBOL) {
      quit(wasm, "Canot bind non-symbol. Got %s, expected %s.",
           lval_type_to_name(lval_sym),
           lval_type_constant_to_name(LVAL_SYMBOL));
    }

    Lval* lval = iter_next(b);
    wasm->is_fn_call = 0;
    Ber wasm_value = lval_compile(wasm, lval).ber;
    Ber local_var = BinaryenLocalSet(
        module, context->function_context->local_count, wasm_value);

    if (wasm->is_fn_call) {
      li_track(wasm, li, li_get(wasm));
      /* local_indices[local_indices_count++] = */
      /*     context->function_context->local_count; */
      /* if (local_indices_count == 128) */
      /*   return quit(wasm, */
      /*               "ERROR: Let can only be invoked with a maximum " */
      /*               "of 128 bindings and body forms that are fn calls%s\n");
       */
    }

    // LENV_PUT
    int local_index = context->function_context->local_count++;
    lenv_put(let_env, retain(lval_sym),
             make_lval_compiler(context, LOCAL, local_index));

    let_body[let_body_count++] = local_var;

    /* if (lval->type == LVAL_ERR) { */
    /*   release(let_env); */
    /*   return lval; */
    /* } */

    lval_sym = iter_next(b);
  }

  Ber let_result = NULL;
  // Body
  if (arg_list->cdr) {
    // We've got forms in the body of let
    do {
      arg_list = arg_list->cdr;
      wasm->is_fn_call = 0;
      Ber ber = lval_compile(wasm, arg_list->car).ber;

      // This is not the last lval in the body of let
      if (arg_list->cdr) {
        // Drop the value, but store it in a local var if it's a fn call so we
        // can release it before returning
        if (wasm->is_fn_call) {
          int local_index = li_track(wasm, li, li_new(wasm));

          ber = BinaryenLocalSet(module, local_index, ber);
        } else {
          ber = BinaryenDrop(module, ber);
        }
        let_body[let_body_count++] = ber;

      } else {
        // ber is the last value in the body of let
        let_result = ber;
        if (!wasm->is_fn_call) {
          Ber retain_operand[1] = {let_result};
          // Retain value when not result of fn call (a literal)
          let_result = BinaryenCall(module, "retain", retain_operand, 1,
                                    BinaryenTypeInt32());
        }
      }
    } while (arg_list->cdr);
  } else {
    // Empty body
    let_result = wasmify_literal(wasm, make_lval_nil()).ber;
    Ber retain_operand[1] = {let_result};
    let_result =
        BinaryenCall(module, "retain", retain_operand, 1, BinaryenTypeInt32());
  }

  Ber release_locals_block = li_release(wasm, li, "release_locals_for_let").ber;
  let_body[let_body_count++] =
      li_result_with_release(wasm, li, "let_body_result", release_locals_block,
                             let_result)
          .ber;

  leave_env(wasm);

  Ber ret = BinaryenBlock(module, uniquify_name(wasm, "let"), let_body,
                          let_body_count, BinaryenTypeInt32());
  li_close(li);
  free(let_body);
  return cresult(ret);
}

CResult compile_if(Wasm* wasm, Cell* args) {
  BinaryenModuleRef module = wasm->module;
  printf("Compiling if!!!!\n");
  if (!args) quit(wasm, "Too few arguments to if");
  Lval* cond = args->car;
  args = args->cdr;
  if (!args) quit(wasm, "Too few arguments to if");
  Lval* if_true = args->car;
  args = args->cdr;
  Lval* if_false = NULL;
  if (args) if_false = args->car;
  if (args && args->cdr) quit(wasm, "Too many arguments to if");
  Ber if_wasm = BinaryenIf(
      module, lval_compile(wasm, cond).ber, lval_compile(wasm, if_true).ber,
      if_false ? lval_compile(wasm, if_false).ber
               : wasmify_literal(wasm, make_lval_nil()).ber);
  return cresult(if_wasm);
}

CResult compile_try(Wasm* wasm, Cell* args) {
  return cresult(make_int32(wasm->module, 0));
}

CResult compile_throw(Wasm* wasm, Cell* args) {
  return cresult(make_int32(wasm->module, 0));
}

CResult compile_quote(Wasm* wasm, Cell* args) {
  if (list_count(args) != 1) {
    quit(wasm, "Wrong number of args (%d) passed to quote, expected 1",
         list_count(args));
  }
  Lval* arg = args->car;
  switch (arg->type) {
    case LVAL_COLLECTION:;
      CResult cresult = wasmify_collection(wasm, arg);
      return cresult;
    case LVAL_SYMBOL:
    case LVAL_LITERAL:
      return wasmify_literal(wasm, arg);
    default:
      return quit(wasm, "Unknown lval type, can't wasmify it for quote fn!!!!");
  }
}

CResult compile_quasiquote(Wasm* wasm, Cell* args) {
  return cresult(make_int32(wasm->module, 0));
}

// Release results of fn calls
/* Ber release_locals[local_indices_count]; */
/* Ber get_local[1]; */
/* for (int i = 0; i < local_indices_count; i++) { */
/*   get_local[0] = */
/*       BinaryenLocalGet(module, local_indices[i], BinaryenTypeInt32()); */
/*   release_locals[i] = */
/*       BinaryenCall(module, "release", get_local, 1, BinaryenTypeNone()); */
/* } */

/* Ber release_locals_block = */
/*     BinaryenBlock(module, uniquify_name(wasm, "release_locals_of_let"), */
/*                   release_locals, local_indices_count, BinaryenTypeNone());
 */

/* int local_index = local_indices[local_indices_count++] = */
/*     context->function_context->local_count++; */
/* if (local_indices_count == 128) */
/*   return quit(wasm, */
/*               "ERROR: Let can only be invoked with a maximum " */
/*               "of 128 bindings and body forms that are fn
 * calls%s\n"); */
