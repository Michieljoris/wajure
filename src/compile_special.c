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
    CResult result = lval_compile(wasm, lval);
    Ber wasm_value = result.ber;
    Ber local_var = BinaryenLocalSet(
        module, context->function_context->local_count, wasm_value);
    /* printf("let lval: is_fn? %d", result.is_fn_call); */
    /* lval_println(lval); */
    if (result.is_fn_call) li_track(wasm, li, li_get(wasm));

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
      CResult result = lval_compile(wasm, arg_list->car);
      Ber ber = result.ber;
      // This is not the last lval in the body of let
      if (arg_list->cdr) {
        // Drop the value, but store it in a local var if it's a fn call so we
        // can release it before returning
        if (result.is_fn_call) {
          int local_index = li_track(wasm, li, li_new(wasm));

          ber = BinaryenLocalSet(module, local_index, ber);
        } else {
          ber = BinaryenDrop(module, ber);
        }
        let_body[let_body_count++] = ber;

      } else {
        // ber is the last value in the body of let
        let_result = ber;
        if (!result.is_fn_call) {
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

CResult is_falsy(Wasm* wasm, Ber wval) {
  int v1 = LNIL;
  int v2 = LFALSE;
  int lower = v1, higher = v2;
  if (v1 > v2) {
    lower = v2;
    higher = v1;
  }
  if ((higher | lower) != higher) {
    printf("LVAL_NIL and LVAL_FALSE should be bit subsets!! %d\n",
           higher & lower);
    abort();
  }
  int magic_value = (higher ^ lower) ^ 255;
  BinaryenModuleRef module = wasm->module;
  Ber subtype = BinaryenLoad(module, 1, 0, 1, 0, BinaryenTypeInt32(), wval);
  Ber and = BinaryenBinary(module, BinaryenAndInt32(), subtype,
                           make_int32(module, magic_value));
  Ber eq =
      BinaryenBinary(module, BinaryenEqInt32(), and, make_int32(module, lower));
  return cresult(eq);
}

CResult is_truthy(Wasm* wasm, Ber wval) {
  BinaryenModuleRef module = wasm->module;
  return cresult(BinaryenBinary(module, BinaryenXorInt32(),
                                is_falsy(wasm, wval).ber,
                                make_int32(module, 1)));
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
  Ber wasm_cond = lval_compile(wasm, cond).ber;
  wasm_cond = is_truthy(wasm, wasm_cond).ber;
  Ber if_wasm =
      BinaryenIf(module, wasm_cond, lval_compile(wasm, if_true).ber,
                 if_false ? lval_compile(wasm, if_false).ber
                          : wasmify_literal(wasm, make_lval_nil()).ber);
  return cresult(if_wasm);
}

CResult compile_try(Wasm* wasm, Cell* args) {
  printf("NOTE!!!!!: try/catch is not implemented yet\n");
  return cresult(make_int32(wasm->module, 0));
}

CResult compile_throw(Wasm* wasm, Cell* args) {
  printf("NOTE!!!!!: throw is not implemented yet\n");
  return cresult(make_int32(wasm->module, 0));
}

CResult compile_quote(Wasm* wasm, Cell* args) {
  if (list_count(args) != 1) {
    quit(wasm, "Wrong number of args (%d) passed to quote, expected 1",
         list_count(args));
  }
  Lval* arg = args->car;
  return wasmify_lval(wasm, arg);
}

CResult compile_quasiquote(Wasm* wasm, Cell* args) {
  printf("NOTE!!!!!: quasiquote is not implemented yet\n");
  return cresult(make_int32(wasm->module, 0));
}
