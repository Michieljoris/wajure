#include "make_wasm_fn.h"

#include "compile.h"
#include "env.h"
#include "lispy_mempool.h"
#include "print.h"
#include "wasm_util.h"

Ber args_into_locals(Wasm* wasm, int min_param_count, int has_rest_arg) {
  BinaryenModuleRef module = wasm->module;
  int args_block_ptr_param = 1;
  int args_count_param = 2;

  Ber* block_operands = malloc((min_param_count + 1) * sizeof(Ber));
  int block_count = 0;
  for (int i = 0; i < min_param_count; i++) {
    int local = li_new(wasm);
    Ber arg = BinaryenLoad(module, 4, 0, i * 4, 2, BinaryenTypeInt32(),
                           local_get_int32(module, args_block_ptr_param));
    block_operands[block_count++] = BinaryenLocalSet(module, local, arg);
  }
  if (has_rest_arg) {
    Ber listify_args_operands[] = {
        BinaryenBinary(module, BinaryenAddInt32(),
                       local_get_int32(module, args_block_ptr_param),
                       make_int32(module, 4 * min_param_count)),
        BinaryenBinary(module, BinaryenSubInt32(),
                       local_get_int32(module, args_count_param),
                       make_int32(module, min_param_count))};
    Ber args_as_list = BinaryenCall(
        module, "listify_args", listify_args_operands, 2, BinaryenTypeInt32());
    int local = li_new(wasm);
    block_operands[block_count++] =
        BinaryenLocalSet(module, local, args_as_list);
  }
  return BinaryenBlock(module, uniquify_name(wasm, "args_into_locals"),
                       block_operands, block_count, BinaryenTypeNone());
}

Ber* add_case(Wasm* wasm, Ber* ber, char* name, Ber result) {
  BinaryenModuleRef module = wasm->module;
  Ber* children = malloc(2 * sizeof(Ber));
  int ber_count = 2;
  children[0] = BinaryenBlock(module, name, ber, ber_count, BinaryenTypeNone());
  free(ber);
  children[1] = BinaryenReturn(module, result);
  return children;
}
Ber switch_on_args_count(Wasm* wasm, Lval* lval_fn, int args_count_param,
                         LambdaBer get_lambda_ber) {
  BinaryenModuleRef module = wasm->module;
  const char* br_table[MAX_FN_PARAMS];
  Lambda** lambdas = lval_fn->lambdas;
  int br_table_count = 0;
  int max_arity = 0;
  int has_rest_arg = 0;
  char* case_names[MAX_FN_PARAMS + 1] = {
      "case0",  "case1",  "case2",  "case3",  "case4",  "case5",  "case6",
      "case7",  "case8",  "case9",  "case10", "case11", "case12", "case13",
      "case14", "case15", "case16", "case17", "case18", "case19", "case20"};

  char* clauses[MAX_FN_PARAMS + 1];
  Ber bers[MAX_FN_PARAMS + 1];
  int clause_count = 0;

  char* default_name = NULL;
  int add_rt_error_clause = 0;
  for (int i = 0; i <= MAX_FN_PARAMS; i++) {
    Lambda* lambda = lambdas[i];
    if (lambda) {
      if (lambda->has_rest_arg) {
        has_rest_arg = 1;
        max_arity = i - 1;
        default_name = clauses[clause_count] = "rest_arg";
        bers[clause_count] = get_lambda_ber(wasm, lval_fn, i);
        clause_count++;
        break;
      } else
        max_arity = i;
    }
  }
  if (!default_name) {
    default_name = "rt_error";
    add_rt_error_clause = 1;
  }

  for (int i = 0; i <= max_arity; i++) {
    Lambda* lambda = lambdas[i];
    if (!lambda) {
      if (i == max_arity && has_rest_arg)
        max_arity--;  // this will be handled by the variadic fn
      else {
        br_table[br_table_count++] = "rt_error";
        add_rt_error_clause = 1;
      }
    } else {
      br_table[br_table_count++] = clauses[clause_count] = case_names[i];
      bers[clause_count] = get_lambda_ber(wasm, lval_fn, i);
      clause_count++;
    }
  }

  if (add_rt_error_clause) {
    clauses[clause_count] = "rt_error";

    CResult rt_error = wasm_runtime_error(wasm, RT_WRONG_NUMBER_OF_ARGS, NULL);
    Ber children[] = {
        wasm_log_int(wasm, local_get_int32(module, args_count_param)),
        rt_error.ber, make_int32(module, 0)};
    bers[clause_count] =
        BinaryenBlock(module, NULL, children, 3, BinaryenTypeInt32());
    clause_count++;
  }

  Ber args_count = local_get_int32(module, args_count_param);
  Ber br_table_ber = BinaryenSwitch(module, br_table, br_table_count,
                                    default_name, args_count, NULL);
  Ber* clause = malloc(2 * sizeof(Ber));
  clause[0] = br_table_ber;
  clause[1] = BinaryenNop(module);

  for (int i = 0; i < clause_count; i++)
    clause = add_case(wasm, clause, clauses[i], bers[i]);

  Ber body = BinaryenBlock(module, NULL, clause, 2, BinaryenTypeInt32());
  return body;
}

FunctionData add_wasm_function(Wasm* wasm, Lenv* env, char* self,
                               Lambda* lambda, Cell* symbol_to_ref,
                               int wajure_args) {
  char* fn_name = lambda->name;
  printf(">>>>>>>> add_wasm_function %s\n", fn_name);
  int param_count = lambda->param_count;
  // The function's closure, args_count_ptr and args_count, or closure and
  // params
  int wasm_params_count = wajure_args == ABI_PARAMS ? param_count : 3;
  CONTEXT_FUNCTION(">>>>>>>add_wasm_function", fn_name, wasm_params_count,
                   symbol_to_ref)

  // This does nothing!!!
  Lenv* current_env = wasm->env;
  wasm->env = env;
  // This does nothing!!!

  // Params
  Lenv* ref_env = enter_env(wasm);

  if (self) {
    printf("setting self lval_ref %s\n", self);
    Lval* lval_sym = make_lval_sym(retain(self));
    lenv_put(ref_env, lval_sym, make_lval_ref(context, SELF, -1));
    release(lval_sym);
  }

  int has_rest_arg = lambda->has_rest_arg;  // 1 based
  int min_param_count = has_rest_arg ? param_count - 1 : param_count;

  int i = 0;
  int first_free_local = wajure_args == ABI_PARAMS ? 0 : 3;
  Cell* param = lambda->params->data.head;
  while (i < min_param_count) {
    Lval* lval_ref = make_lval_ref(context, PARAM, first_free_local + i);
    Lval* lval_sym = param->car;
    lenv_put(ref_env, lval_sym, lval_ref);
    release(lval_ref);
    param = param->cdr;
    i++;
  }

  if (has_rest_arg) {
    param = param->cdr;
    Lval* lval_ref = make_lval_ref(context, PARAM, first_free_local + i);
    Lval* lval_sym = param->car;
    lenv_put(ref_env, lval_sym, lval_ref);
  }
  /* env_print(params_env); */

  Ber init = wajure_args == ABI_PARAMS
                 ? NULL
                 : args_into_locals(wasm, min_param_count, has_rest_arg);
  // Compile body
  Ber body = compile_do_list(wasm, lambda->body, init).ber;

  // Post compile
  leave_env(wasm);

  // TODO: compare lval_fn with other fns added so far. If a match don't create
  // the wasm fn again but retrieve its fn-table_index and set that on lval_fun
  // This can happen when macros return a fn for instance.
  int fn_table_index;
  /* int found = find_wasm_fn(lval_fun); */
  /* if (found) { */
  /*   fn_table_index = found; */
  /* } else { */
  int locals_count = context->function_context->local_count - wasm_params_count;

  // TODO: not sure what to do about fn_name....
  fn_table_index = add_fn_to_table(wasm, fn_name);

  BinaryenModuleRef module = wasm->module;

  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(1);

  BinaryenType* local_types = make_type_int32_array(locals_count);

  // Add fn to wasm
  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, body);
  /* } */

  FunctionData function_data = {context->function_context->symbol_to_ref,
                                fn_table_index};

  // THIS does nothing!!!!
  wasm->env = current_env;
  // THIS does nothing!!!!

  return function_data;
}

Operands make_fn_call_operands(Wasm* wasm, Lambda* lambda) {
  BinaryenModuleRef module = wasm->module;

  // The actual root fn takes closure ptr and args matching the params of the
  // original wajure fn.
  Ber* operands = malloc((1 + lambda->param_count) * sizeof(Ber));
  int operands_count = 0;

  // Closure pointer (NULL)
  /* operands[operands_count++] = make_int32(module, 0); */

  int args_block_ptr_param = 1;
  int args_count_param = 2;
  int has_rest_arg = lambda->has_rest_arg;  // 1 based
  int param_count = lambda->param_count;
  int min_param_count = has_rest_arg ? param_count - 1 : param_count;

  // Load all non variadic args into operands
  for (int i = 0; i < min_param_count; i++) {
    Ber arg = BinaryenLoad(module, 4, 0, i * 4, 2, BinaryenTypeInt32(),
                           local_get_int32(module, args_block_ptr_param));
    operands[operands_count++] = arg;
  }

  // Load any extra args into variadic arg.
  if (has_rest_arg) {
    Ber listify_args_operands[] = {
        BinaryenBinary(module, BinaryenAddInt32(),
                       local_get_int32(module, args_block_ptr_param),
                       make_int32(module, (has_rest_arg - 1) * 4)),
        BinaryenBinary(module, BinaryenSubInt32(),
                       local_get_int32(module, args_count_param),
                       make_int32(module, has_rest_arg - 1))};
    Ber args_as_list = BinaryenCall(
        module, "listify_args", listify_args_operands, 2, BinaryenTypeInt32());
    operands[operands_count++] = args_as_list;
  }
  Operands ret = {.operands = operands, .count = operands_count};
  return ret;
}

char* add_arity_to_fn_name(char* fn_name, int index) {
  char* wajure_fn_name = malloc(_strlen(fn_name) + number_len(index) + 3);
  sprintf(wajure_fn_name, "%s_a%d", fn_name, index);
  return wajure_fn_name;
}

// This adds a wasm fn that has as params: [closure, param 1, param 2, etc].
// When we call this fn directly (by name) we can pass our compiled args
// directly to the fn.
Ber make_root_fn(Wasm* wasm, Lval* lval_fn, int arity) {
  BinaryenModuleRef module = wasm->module;

  // Add the fn for the arity to wasm
  char* wajure_fn_name = add_arity_to_fn_name(lval_fn->cname, arity);
  Lambda* lambda = lval_fn->lambdas[arity];
  lambda->name = wajure_fn_name;
  printf("make_root_fn, lambda_name is: %s\n", wajure_fn_name);
  FunctionData function_data =
      add_wasm_function(wasm, lval_fn->closure, NULL, lambda, NULL, ABI_PARAMS);

  // This is so that we can call this fn using params (iso args_block)
  // indirectly from other namespaces
  write_symbol_table_line(wasm, LVAL_FUNCTION, wajure_fn_name, -1,
                          function_data.fn_table_index, lambda->param_count,
                          lambda->has_rest_arg);

  // Call the root fn with actual params (not args_block)
  Operands operands = make_fn_call_operands(wasm, lambda);
  Ber call_fn = BinaryenCall(module, wajure_fn_name, operands.operands,
                             operands.count, BinaryenTypeInt32());
  return call_fn;
}

int add_root_fn(Wasm* wasm, Lval* lval_fn) {
  // Build a fn that takes closure, args_block_ptr and args_count and calls the
  // actual root fn.
  BinaryenModuleRef module = wasm->module;
  int args_count_param = 2;

  Ber branch_on_arity =
      switch_on_args_count(wasm, lval_fn, args_count_param, make_root_fn);
  // Add the function to wasm
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(1);

  BinaryenAddFunction(module, lval_fn->cname, params_type, results_type, NULL,
                      0, branch_on_arity);

  return add_fn_to_table(wasm, lval_fn->cname);
}

Ber make_local_lambda(Wasm* wasm, Lval* lval_fn, int arity) {
  printf("make_local_lambda\n");
  Context* context = wasm->context->car;
  BinaryenModuleRef module = wasm->module;
  char* lambda_name = add_arity_to_fn_name(lval_fn->cname, arity);

  Lambda* lambda = lval_fn->lambdas[arity];  // TODO!!!!!!!!
  lambda->name = lambda_name;

  char* self = lval_fn->data.str;
  FunctionData function_data = add_wasm_function(wasm, wasm->env, self, lambda,
                                                 context->cell, ABI_ARGS_BLOCK);
  context->cell = function_data.symbol_to_ref;

  // Call the root fn with actual params (not args_block)
  Ber operands[] = {local_get_int32(module, 0), local_get_int32(module, 1),
                    local_get_int32(module, 2)};
  Ber call_fn =
      BinaryenCall(module, lambda_name, operands, 3, BinaryenTypeInt32());
  free(lambda_name);
  return call_fn;
}

FunctionData add_local_lambda(Wasm* wasm, Lval* lval_fn) {
  CONTEXT_LVAL("context: add_local_lambda", lval_fn)
  /* print_context(context); */

  // Build a fn that takes closure, args_block_ptr and args_count and calls the
  // actual root fn.
  BinaryenModuleRef module = wasm->module;
  int args_count_param = 2;

  Ber branch_on_arity =
      switch_on_args_count(wasm, lval_fn, args_count_param, make_local_lambda);

  // Add the function to wasm
  BinaryenType params_type = make_type_int32(3);
  BinaryenType results_type = make_type_int32(1);

  BinaryenAddFunction(module, lval_fn->cname, params_type, results_type, NULL,
                      0, branch_on_arity);

  FunctionData function_data = {
      .fn_table_index = add_fn_to_table(wasm, lval_fn->cname),
      .symbol_to_ref = retain(context->cell)};
  return function_data;
}
