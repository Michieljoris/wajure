#include "compile_fn_call.h"

#include "compile.h"
#include "datafy.h"
#include "eval.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "make_wasm_fn.h"
#include "namespace.h"
#include "native.h"
#include "print.h"
#include "refcount.h"
#include "state.h"
#include "wasm.h"
#include "wasm_util.h"

Ber compile_args_into_block(Wasm* wasm, int wval_local, int partial_count_local,
                            Cell* args, int args_count,
                            int args_block_ptr_local, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;
  Ber* children = malloc((3 + args_count) * sizeof(Ber));
  /* printf("CHILDREN COUNT: %d\n", 2 + args_count); */
  int children_count = 0;

  // Size of partials on block
  Ber size = BinaryenBinary(module, BinaryenMulInt32(),
                            local_get_int32(module, partial_count_local),
                            make_int32(module, 4));

  // Copy partials to start of args_block
  int size_local = li_new(wasm);

  // if partial_count != 0 then copy partials of wval to where args_block_ptr
  // points

  Ber mem_copy = BinaryenMemoryCopy(
      module, local_get_int32(module, args_block_ptr_local),
      get_wval_prop(module, local_get_int32(module, wval_local), "partials"),
      BinaryenLocalTee(module, size_local, size, BinaryenTypeInt32()));

  int args_block_plus_partials_ptr_local;
  if (!args_count) {
    children[children_count++] =
        BinaryenIf(module, local_get_int32(module, partial_count_local),
                   mem_copy, BinaryenNop(module));
  } else {
    // Add size of partials to arg block ptr
    Ber add_size = BinaryenBinary(module, BinaryenAddInt32(),
                                  local_get_int32(module, args_block_ptr_local),
                                  local_get_int32(module, size_local));
    Ber children2[] = {mem_copy, add_size};
    Ber block = BinaryenBlock(module, NULL, children2, 2, BinaryenTypeInt32());
    Ber if_partials =
        BinaryenIf(module, local_get_int32(module, partial_count_local), block,
                   local_get_int32(module, args_block_ptr_local));

    args_block_plus_partials_ptr_local = li_new(wasm);
    children[children_count++] = BinaryenLocalSet(
        module, args_block_plus_partials_ptr_local, if_partials);
  }
  int offset = 0;
  // Put args on block ====================================================
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;
    // Any arg that's a result of a fn call will be released
    if (result.is_fn_call) {
      int local_index = li_track(wasm, li, li_new(wasm));
      compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                      BinaryenTypeInt32());
    }
    // Put compiled arg on args block
    Ber args_block_ptr =
        local_get_int32(module, args_block_plus_partials_ptr_local);
    children[children_count++] =
        BinaryenStore(module, 4, offset, 2, args_block_ptr, compiled_arg,
                      BinaryenTypeInt32());
    // And loop
    offset += 4;
    args = args->cdr;
  }
  Ber block = BinaryenBlock(module, uniquify_name(wasm, "args"), children,
                            children_count, BinaryenTypeNone());
  free(children);
  return block;
}

Ber call_fn_by_ref(Wasm* wasm, Ber wval, Cell* args, int args_block_ptr_local,
                   Ber* block_children, int* block_children_count,
                   LocalIndices* li) {
  /* printf("call_fn_by_ref\n"); */
  BinaryenModuleRef module = wasm->module;

  // Store result of computation of our fn in a local
  int wval_local = li_new(wasm);  // TODO: use tee in first use
  block_children[(*block_children_count)++] =
      BinaryenLocalSet(module, wval_local, wval);

  // Add partial count and args count together for total args count
  int partial_count_local = li_new(wasm);
  Ber partial_count = get_wval_prop(module, local_get_int32(module, wval_local),
                                    "partial_count");
  int args_count = list_count(args);
  int total_args_count_local = li_new(wasm);

  Ber total_args_count =
      BinaryenBinary(module, BinaryenAddInt32(), make_int32(module, args_count),
                     BinaryenLocalTee(module, partial_count_local,
                                      partial_count, BinaryenTypeInt32()));
  total_args_count = BinaryenLocalTee(module, total_args_count_local,
                                      total_args_count, BinaryenTypeInt32());
  Ber operands[] = {BinaryenBinary(module, BinaryenMulInt32(), total_args_count,
                                   make_int32(module, 4))};
  // Allocate a block of memory to copy the arg ptrs into
  Ber args_block_ptr =
      BinaryenCall(module, "lalloc_size", operands, 1, BinaryenTypeInt32());

  // And put the ptr to this block into a local
  Ber local_set_args_block_ptr =
      BinaryenLocalSet(module, args_block_ptr_local, args_block_ptr);
  block_children[(*block_children_count)++] = local_set_args_block_ptr;

  // Load partials and args into the args block
  Ber args_block =
      compile_args_into_block(wasm, wval_local, partial_count_local, args,
                              args_count, args_block_ptr_local, li);
  block_children[(*block_children_count)++] = args_block;
  Ber closure_pointer =
      get_wval_prop(module, local_get_int32(module, wval_local), "closure");
  Ber call_operands[] = {closure_pointer,
                         local_get_int32(module, args_block_ptr_local),
                         local_get_int32(module, total_args_count_local)};

  Ber fn_table_index = get_wval_prop(
      module, local_get_int32(module, wval_local), "fn_table_index");
  return BinaryenCallIndirect(module, fn_table_index, call_operands, 3,
                              make_type_int32(3), BinaryenTypeInt32());
}

Operands compile_args_into_operands(Wasm* wasm, char* fn_name, Lval* lval_fn,
                                    Cell* args, LocalIndices* li) {
  BinaryenModuleRef module = wasm->module;

  int args_count = list_count(args) + list_count(lval_fn->partials);
  int arity = min(args_count, MAX_FN_PARAMS);
  Lambda* lambda = lval_fn->lambdas[arity];
  if (!lambda)
    quit(wasm, "Wrong number of args (%d) passed to %s", arity, lval_fn->cname);

  int param_count = lambda->param_count;
  int has_rest_arg = lambda->has_rest_arg;
  int min_param_count = has_rest_arg ? param_count - 1 : param_count;
  int partial_count = list_count(lval_fn->partials);
  int total_args_count = args_count + partial_count;

  Ber* compiled_args = malloc(sizeof(Ber) * total_args_count);
  int compiled_args_count = 0;

  // Grab and datafy the partials from the function
  Cell* head = lval_fn->partials;
  while (head) {
    Ber compiled_arg = datafy_lval(wasm, head->car).ber;
    // We need to retain datafied values if the value ends up in the rest arg
    // because it will be released when we're done with this fn call
    if (compiled_args_count >= min_param_count)
      compiled_arg = wasm_retain(wasm, compiled_arg).ber;
    /* printf("partial compiled_arg %d :", compiled_args_count); */
    /* lval_println(head->car); */
    compiled_args[compiled_args_count++] = compiled_arg;
    head = head->cdr;
  }

  // And grab and compile the args
  head = args;
  while (head) {
    /* lval_println(head->car); */
    CResult result = lval_compile(wasm, head->car);
    Ber compiled_arg = result.ber;
    if (compiled_args_count < min_param_count) {
      // If the arg is not part of the rest arg we'll release it when we're done
      // with the fn call
      if (result.is_fn_call) {
        int local_index = li_track(wasm, li, li_new(wasm));
        compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
                                        BinaryenTypeInt32());
      }
    } else {
      // If the arg *is* part of the rest arg we retain any data values, since
      // the rest arg list will be released
      if (!result.is_fn_call)
        compiled_arg = wasm_retain(wasm, compiled_arg).ber;
    }
    /* printf("regular compiled_arg %d :", compiled_args_count); */
    /* lval_println(head->car); */
    compiled_args[compiled_args_count++] = compiled_arg;
    head = head->cdr;
  }

  // Let's make the operands for the fn call
  Ber* call_operands = malloc((1 + param_count) * sizeof(Ber));
  // First arg is always a closure pointer
  Ber closure_pointer = make_int32(module, 0);
  int call_operands_count = 0;
  call_operands[call_operands_count++] = closure_pointer;

  // Add all the args that are definitely not part of any rest arg.
  for (int i = 0; i < min_param_count; i++)
    call_operands[call_operands_count++] = compiled_args[i];

  // Collect the remainder in the rest arg
  if (has_rest_arg) {
    int rest_arg_count = total_args_count - min_param_count;
    if (rest_arg_count) {
      // Make a chain of cells, one cell per extra arg
      Ber head = make_int32(module, 0);
      int i = total_args_count - 1;
      do {
        Ber operands[] = {compiled_args[i], head};
        /* printf("Making new cell %d\n ", i); */
        head =
            BinaryenCall(module, "new_cell", operands, 2, BinaryenTypeInt32());
        i--;
      } while (i >= min_param_count);
      // Assign the head to a new list
      Ber lval_list_operands[1] = {head};
      // Make sure it gets released when we're done
      int wasm_lval_list_index = li_track(wasm, li, li_new(wasm));
      Ber wasm_lval_list = BinaryenCall(
          module, "new_lval_list", lval_list_operands, 1, make_type_int32(1));
      wasm_lval_list = BinaryenLocalTee(module, wasm_lval_list_index,
                                        wasm_lval_list, BinaryenTypeInt32());
      // And add the rest arg list to our operands
      call_operands[call_operands_count++] = wasm_lval_list;
    } else
      // Put a nil in the rest arg if there's no args left
      call_operands[call_operands_count++] = datafy_nil(wasm).ber;
  }
  free(compiled_args);

  Operands ret = {call_operands, call_operands_count};
  return ret;
}

Ber call_fn_by_name(Wasm* wasm, char* fn_name, Lval* lval_fn, Cell* args,
                    LocalIndices* li) {
  /* printf("call_fn_by_name %s\n", fn_name); */
  Operands operands =
      compile_args_into_operands(wasm, fn_name, lval_fn, args, li);
  Ber result = BinaryenCall(wasm->module, fn_name, operands.operands,
                            operands.count, BinaryenTypeInt32());
  free(operands.operands);
  return result;
}

Ber call_indirect(Wasm* wasm, Ber fn_table_index, Lval* lval_fn, Cell* args,
                  LocalIndices* li) {
  /* printf("call_indirect (external or partial fn or both)\n"); */
  Operands operands =
      compile_args_into_operands(wasm, lval_fn->cname, lval_fn, args, li);
  Ber result = BinaryenCallIndirect(
      wasm->module, fn_table_index, operands.operands, operands.count,
      make_type_int32(operands.count), BinaryenTypeInt32());
  free(operands.operands);
  return result;
}

CResult apply(Wasm* wasm, int fn_ref_type, union FnRef fn_ref, Lval* lval_fn,
              Cell* args) {
  BinaryenModuleRef module = wasm->module;

  int block_children_count = 0;

  // Put partials on stack, args, release_locals, result
  int block_size = 10;  // TODO: 10 ?????????????????
  /* int block_size = 99;       // TODO: for debugging */
  int max_block_children_count = list_count(args) + block_size;
  Ber* block_children = malloc(sizeof(Ber) * max_block_children_count);

  LocalIndices* li = li_init();

  int args_block_ptr_local;
  // Call fn ================================================================
  Ber result = NULL;
  switch (fn_ref_type) {
    case BER: {
      Ber wval = fn_ref.ber;
      args_block_ptr_local = li_new(wasm);
      result = call_fn_by_ref(wasm, wval, args, args_block_ptr_local,
                              block_children, &block_children_count, li);

    } break;
    case FN_NAME: {
      result = call_fn_by_name(wasm, fn_ref.fn_name, lval_fn, args, li);
    } break;
    case INDIRECT_EXTERNAL: {
      char* fn_table_index_external = fn_ref.global_name;
      Ber fn_table_index = BinaryenGlobalGet(
          wasm->module, fn_table_index_external, BinaryenTypeInt32());
      result = call_indirect(wasm, fn_table_index, lval_fn, args, li);
    } break;
    default:
      quit(wasm, "Unknown fn_ref_type %d", fn_ref_type);
  }

  // Store fn call result in local wasm var
  // =========================================
  int result_local = li_new(wasm);
  block_children[block_children_count++] =
      BinaryenLocalSet(module, result_local, result);

  // Release results of fn calls =============================================
  Ber release_locals_block =
      li_release(wasm, li, "release_locals_for_fn_call").ber;
  if (release_locals_block)
    block_children[block_children_count++] = release_locals_block;

  if (fn_ref_type == BER) {
    Ber args_block_ptr = local_get_int32(module, args_block_ptr_local);
    block_children[block_children_count++] =
        wasm_release(wasm, args_block_ptr).ber;
  }

  // Get result from local wasm var =========================================
  Ber result_from_local = local_get_int32(module, result_local);
  block_children[block_children_count++] = result_from_local;

  // Call block ============================================================
  Ber call_block =
      BinaryenBlock(module, uniquify_name(wasm, "lambda_call"), block_children,
                    block_children_count, BinaryenTypeInt32());

  free(block_children);
  li_close(li);
  return cresult(call_block);
}

CResult compile_as_fn_call(Wasm* wasm, Lval* lval, Cell* args) {
  switch (lval->type) {
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:;
          Ber compiled_list = compile_application(wasm, lval).ber;
          int anon_lambda_index = li_new(wasm);
          Ber local_set_anon_lambda =
              BinaryenLocalSet(wasm->module, anon_lambda_index, compiled_list);
          Ber local_get_anon_lambda = BinaryenLocalGet(
              wasm->module, anon_lambda_index, BinaryenTypeInt32());
          int fn_call_result_index = li_new(wasm);
          union FnRef fn_ref = {.ber = local_get_anon_lambda};
          Ber fn_call = apply(wasm, BER, fn_ref, NULL, args).ber;
          Ber local_set_fn_result =
              BinaryenLocalSet(wasm->module, fn_call_result_index, fn_call);
          Ber local_get_fn_result = BinaryenLocalGet(
              wasm->module, fn_call_result_index, BinaryenTypeInt32());
          Ber release_anon_lambda =
              wasm_release(wasm, local_get_anon_lambda).ber;
          Ber children[] = {local_set_anon_lambda, local_set_fn_result,
                            release_anon_lambda, local_get_fn_result};

          Ber block =
              BinaryenBlock(wasm->module, uniquify_name(wasm, "anon-lambda"),
                            children, 4, BinaryenTypeInt32());
          return cresult(block);
        case VECTOR:
        case MAP:
        case SET:
          quit(wasm, "ERROR: Map/set/vector cannot be cast as fn (yet)\n");
        default:
          quit(wasm, "ERROR: Unknown collection subtype %d", lval->subtype);
      }
      break;
    case LVAL_LITERAL:
      quit(wasm,
           "ERROR: Literal or symbol cannot be cast as fn (not even a "
           "keyword, "
           "yet)\n");
    case LVAL_ERR:
      quit(wasm, "ERROR compile_list: there was an error in parsing code: %s\n",
           lval->data.str);
    default:
      quit(wasm, "ERROR compile_list: Unknown lval type %d\n", lval->type);
  }
  return cnull();
}

// A non empty list is taken to be a fn call. We do our best to resolve the
// expr in the list as a fn call, and pass the rest of the list as args.
CResult compile_application(Wasm* wasm, Lval* lval_list) {
  CONTEXT_LVAL("apply", lval_list);
  /* printf("compile_application\n"); */
  /* lval_println(lval_list); */

  // Empty list
  if (lval_list->data.head == NIL) return datafy_empty_list(wasm, lval_list);

  Lval* lval_applicator = lval_list->data.head->car;
  Cell* args = lval_list->data.head->cdr;
  CResult fn_call = cnull();

  // Fn call > (symbol args ...)
  if (lval_applicator->type == LVAL_SYMBOL) {
    Lval* lval_sym = lval_applicator;
    // Let's see if the symbol refers to something we know how to compile
    // already
    Lval* resolved_sym = eval_symbol(wasm->env, lval_sym);
    // If it's a symbol it has to be known in our compiler env!!!
    if (resolved_sym->type == LVAL_ERR) {
      lval_println(lval_list);
      lval_println(lval_sym);
      quit(wasm, "ERROR: Can't apply an unknowm symbol: %s",
           lval_sym->data.str);
    }

    switch (resolved_sym->type) {
      case LVAL_FUNCTION:  // as evalled in our compiler env
        switch (resolved_sym->subtype) {
          case SYS:;
            Cell* all_args = list_concat(resolved_sym->partials, args);
            fn_call = compile_sys_call(wasm, resolved_sym, all_args);
            release(all_args);
            break;
          case SPECIAL:
            fn_call = compile_special_call(wasm, resolved_sym, args);
            break;
          case LAMBDA:;  // root functions in compiler env
            int lval_is_external =
                resolved_sym->ns && resolved_sym->ns != get_current_ns() ? 1
                                                                         : 0;
            Lval* cfn = resolved_sym->cfn;
            int args_count = list_count(args);
            args_count += list_count(resolved_sym->partials);
            if (cfn) {
              printf("args_count: %d\n", args_count);
              char* wajure_fn_name =
                  add_arity_to_fn_name(cfn->cname, args_count);
              if (cfn->ns == get_current_ns()) {
                // Partial fn, derived from cfn
                /* union FnRef fn_ref = {.fn_name = cfn->cname}; */
                union FnRef fn_ref = {.fn_name = wajure_fn_name};
                fn_call = apply(wasm, FN_NAME, fn_ref, resolved_sym, args);
                free(wajure_fn_name);
              } else {
                char* global_name =
                    make_global_name("fn:", cfn->ns->namespace, wajure_fn_name);
                /* make_global_name("fn:", cfn->ns->namespace, cfn->cname); */
                add_dep(wasm, global_name);
                union FnRef fn_ref = {.global_name = global_name};
                fn_call =
                    apply(wasm, INDIRECT_EXTERNAL, fn_ref, resolved_sym, args);
                free(wajure_fn_name);
                release(global_name);
              }
            } else if (lval_is_external) {  // required from another namespace
              char* wajure_fn_name =
                  add_arity_to_fn_name(resolved_sym->cname, args_count);
              char* global_name = make_global_name(
                  "fn:", resolved_sym->ns->namespace, wajure_fn_name);
              /* char* global_name = make_global_name( */
              /*     "fn:", resolved_sym->ns->namespace, resolved_sym->cname);
               */
              add_dep(wasm, global_name);
              union FnRef fn_ref = {.global_name = global_name};
              fn_call =
                  apply(wasm, INDIRECT_EXTERNAL, fn_ref, resolved_sym, args);
              free(wajure_fn_name);
              release(global_name);
            } else {  // local fn
              char* wajure_fn_name =
                  add_arity_to_fn_name(resolved_sym->cname, args_count);
              union FnRef fn_ref = {.fn_name = wajure_fn_name};
              /* union FnRef fn_ref = {.fn_name = resolved_sym->cname}; */
              fn_call = apply(wasm, FN_NAME, fn_ref, resolved_sym, args);
              free(wajure_fn_name);
            }
            fn_call.is_fn_call = 1;
            break;
          case MACRO:;
            Lval* arg_list = make_lval_list();
            arg_list->data.head = retain(args);
            Lval* bound_macro = expand_macro(resolved_sym, arg_list);
            release(arg_list);
            if (bound_macro->type == LVAL_ERR)
              quit(wasm, "Error in expanding macro");
            CResult result = lval_compile(wasm, bound_macro);
            release(bound_macro);
            return result;
          default:
            quit(wasm,
                 "ERROR: Can't compile function with unknown subtype %d\n",
                 lval_applicator->subtype);
        }
        break;
      case LVAL_REF:;
        // we've assigned something to a local or closed over ref in wasm, but
        // we don't know what it is, and whether it's a fn even. It'll have to
        // be worked out at runtime. With some clever optimisations we can in
        // some cases probably work out what it is at compile time and can
        // reduce it to a compile_list/lambda_call. However the default case
        // is that we cannot assume anything about the lval.

        Ber compiled_symbol =
            compile_lval_ref(wasm, lval_applicator, resolved_sym).ber;

        union FnRef fn_ref = {.ber = compiled_symbol};
        fn_call = apply(wasm, BER, fn_ref, NULL, args);
        fn_call.is_fn_call = 1;
        break;
      case LVAL_SYMBOL:
        quit(wasm, "ERROR: A symbol can't be cast to a fn: %s",
             resolved_sym->data.str);
      case LVAL_COLLECTION:
        switch (resolved_sym->subtype) {
          case LIST:
            quit(wasm, "ERROR: A list can't be cast to a fn");
          default:;
        }
        // Fall through.
      default:;
        // Not a function or wasm ref, perhaps it's something else we can use
        // as a fn, such as a vector or kw or set
        fn_call = compile_as_fn_call(wasm, resolved_sym, args);
        fn_call.is_fn_call = 1;
    }
  } else {
    // Not a function or wasm ref, perhaps it's something else we can use as a
    // fn, such as a vector, or kw or set, or perhaps an anonymous lambda eg:
    // (fn [x] x)
    // TODO: make sure is_fn_call is set properly when we use a map as a fn!!!
    fn_call = compile_as_fn_call(wasm, lval_applicator, args);
    fn_call.is_fn_call = 1;
  }

  return fn_call;
}

/* CResult wasm_process_args(Wasm* wasm, int param_count, int rest_arg_index)
 * {
 */
/*   BinaryenModuleRef module = wasm->module; */
/*   Context* context = wasm->context->car; */
/*   char* fn_name = context->function_context->fn_name; */
/*   Ber args_count = BinaryenLocalGet( */
/*       wasm->module, 1, */
/*       BinaryenTypeInt32());  // second param to fn is args count */

/*   Ber check_args_count = NULL; */
/*   if (wasm->runtime_check_args_count && rest_arg_index != 1) { */
/*     Ber wasm_param_count = make_int32(module, param_count); */
/*     Ber check_operands[3] = {wasm_param_count, args_count, */
/*                              make_int32(module, rest_arg_index)}; */
/*     Ber check_args_count_call_result = BinaryenCall( */
/*         module, "check_args_count", check_operands, 3,
 * BinaryenTypeInt32());
 */
/*     BinaryenOp eq = BinaryenEqInt32(); */
/*     Ber is_too_few_args = */
/*         BinaryenBinary(module, eq, check_args_count_call_result, */
/*                        make_int32(module, TOO_FEW_ARGS)); */
/*     Ber is_too_many_args = */
/*         BinaryenBinary(module, eq, check_args_count_call_result, */
/*                        make_int32(module, TOO_MANY_ARGS)); */
/*     Ber all_ok = BinaryenNop(module); */
/*     Ber if_too_few = BinaryenIf( */
/*         module, is_too_few_args, */
/*         wasm_runtime_error(wasm, RT_TOO_FEW_ARGS, fn_name).ber, all_ok); */
/*     Ber if_too_many = BinaryenIf( */
/*         module, is_too_many_args, */
/*         wasm_runtime_error(wasm, RT_TOO_MANY_ARGS, fn_name).ber,
 * if_too_few);
 */

/*     check_args_count = if_too_many; */
/*   } */

/*   Ber init_rest_arg = NULL; */
/*   if (rest_arg_index) { */
/*     Ber offset = BinaryenBinary(module, BinaryenMulInt32(), */
/*                                 make_int32(module, 4), args_count); */

/*     Ber stack_pointer = */
/*         BinaryenGlobalGet(wasm->module, "stack_pointer",
 * BinaryenTypeInt32()); */
/*     Ber args_start = */
/*         BinaryenBinary(module, BinaryenSubInt32(), stack_pointer, offset);
 */

/*     Ber rest_arg_length = */
/*         BinaryenBinary(module, BinaryenSubInt32(), args_count, */
/*                        make_int32(module, rest_arg_index - 1)); */
/*     Ber operands[] = {args_start, rest_arg_length}; */
/*     init_rest_arg = BinaryenCall(module, "bundle_rest_args", operands, 2,
 */
/*                                  BinaryenTypeNone()); */
/*     if (check_args_count == NULL) return cresult(init_rest_arg); */
/*   } */
/*   if (check_args_count) { */
/*     if (!init_rest_arg) return cresult(check_args_count); */
/*     Ber block_children[2] = {check_args_count, init_rest_arg}; */
/*     return cresult(BinaryenBlock(module, "process_args", block_children, 2,
 */
/*                                  BinaryenTypeNone())); */
/*   } */
/*   return cnull(); */
/* } */

/* Ber get_type_call = get_wval_prop(module, wval, "type"); */
/* block_children[block_children_count++] = */
/*     wasm_log_int(wasm, get_type_call); */

/* printf("Calling lambda!! %s\n", lval_first->str); */
/* printf("Current ns: %s\n", state->current_ns->namespace); */
/* printf("lambda ns: %s\n", resolved_symbol->ns->namespace); */
/* printf("lambda ns name: %s\n", resolved_symbol->name); */
/* printf("APPLY LAMBDA %s :", lval_first->str); */
/* lval_println(resolved_symbol); */

/* Ber call_bundle_rest_args(Wasm* wasm, int wval_local, int
 * args_block_ptr_local, */
/*                           int total_args_count_local) { */
/*   BinaryenModuleRef module = wasm->module; */
/*   Ber operands[] = {local_get_int32(module, wval_local), */
/*                     local_get_int32(module, args_block_ptr_local), */
/*                     local_get_int32(module, total_args_count_local)}; */
/*   return BinaryenCall(module, "bundle_rest_args2", operands, 3, */
/*                       BinaryenTypeNone()); */
/* } */

/* Ber call_bundle_rest_args(Wasm* wasm, int args_block_ptr_index, Ber
 * args_count, */
/*                           Ber wval_rest_arg_index) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   int local_index = li_new(wasm); */
/*   wval_rest_arg_index = BinaryenLocalTee( */
/*       module, local_index, wval_rest_arg_index, BinaryenTypeInt32()); */
/*   Ber offset = BinaryenBinary(module, BinaryenMulInt32(), make_int32(module,
 * 4), */
/*                               wval_rest_arg_index); */

/*   Ber args_block_ptr = */
/*       BinaryenLocalGet(module, args_block_ptr_index, BinaryenTypeInt32()); */
/*   Ber rest_args_start = */
/*       BinaryenBinary(module, BinaryenAddInt32(), args_block_ptr, offset); */

/*   Ber get_wval_rest_arg_index = */
/*       BinaryenLocalGet(module, local_index, BinaryenTypeInt32()); */
/*   Ber rest_arg_length = BinaryenBinary(module, BinaryenSubInt32(),
 * args_count, */
/*                                        get_wval_rest_arg_index); */
/*   Ber operands[] = {rest_args_start, rest_arg_length}; */
/*   return BinaryenCall(module, "bundle_rest_args", operands, 2, */
/*                       BinaryenTypeNone()); */
/* } */

/* const char* names[2] = {"foo", "bar"}; */
/* block_children[block_children_count++] = */
/*     BinaryenSwitch(module, names, 2, "default", make_int32(module, 0), */
/*                    make_int32(module, 1)); */
