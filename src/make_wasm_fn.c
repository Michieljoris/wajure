#include "make_wasm_fn.h"

#include "compile.h"
#include "env.h"
#include "lispy_mempool.h"
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
        local_get_int32(module, args_block_ptr_param),
        local_get_int32(module, args_count_param)};
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

FunctionData add_wasm_function(Wasm* wasm, Lenv* env, char* fn_name,
                               Lambda* lambda, int wajure_args) {
  printf(">>>>>>>> add_wasm_function %s\n", fn_name);
  int param_count = lambda->param_count;
  // The function's closure, args_count_ptr and args_count, or closure and
  // params
  int wasm_params_count = 1 + (wajure_args == ABI_PARAMS ? param_count : 2);
  CONTEXT_FUNCTION(">>>>>>>add_wasm_function", fn_name, wasm_params_count)

  Lenv* current_env = wasm->env;
  wasm->env = env;

  // Params
  Lenv* params_env = enter_env(wasm);

  int has_rest_arg = lambda->has_rest_arg;  // 1 based
  int min_param_count = has_rest_arg ? param_count - 1 : param_count;

  int i = 0;
  int first_free_local = wajure_args == ABI_PARAMS ? 1 : 3;
  Cell* param = lambda->params->data.head;
  while (i < min_param_count) {
    Lval* lval_ref = make_lval_ref(context, PARAM, first_free_local + i);
    Lval* lval_sym = param->car;
    lenv_put(params_env, lval_sym, lval_ref);
    release(lval_ref);
    param = param->cdr;
    i++;
  }

  if (has_rest_arg) {
    param = param->cdr;
    Lval* lval_ref = make_lval_ref(context, PARAM, first_free_local + i);
    Lval* lval_sym = param->car;
    lenv_put(params_env, lval_sym, lval_ref);
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

  /* lval_fun->offset = fn_table_index; */
  FunctionData function_data = {context->function_context->closure,
                                context->function_context->closure_count,
                                fn_table_index};

  wasm->env = current_env;
  return function_data;
}
