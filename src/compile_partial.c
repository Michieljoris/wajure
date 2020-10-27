#include "compile_partial.h"

#include "compile.h"
#include "compile_fn_call.h"
#include "compile_special.h"
#include "datafy.h"
#include "env.h"
#include "eval.h"
#include "fns.h"
#include "inter.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "namespace.h"
#include "print.h"
#include "read.h"
#include "refcount.h"
#include "run.h"
#include "runtime.h"
#include "special_fns.h"
#include "state.h"
#include "wasm.h"
#include "wasm_util.h"

CResult test_native_partial_fn(Wasm* wasm, CResult fn_arg, Cell* args,
                               int args_count) {
  BinaryenModuleRef module = wasm->module;
  /* LocalIndices* li = li_init(); */
  int offset = 0;

  // Allocate an arg block
  Ber operands[] = {make_int32(module, args_count * 4)};
  Ber args_block_ptr =
      BinaryenCall(module, "lalloc_size", operands, 1, BinaryenTypeInt32());
  int args_block_ptr_local = li_new(wasm);
  Ber local_tee_args_block_ptr = BinaryenLocalTee(
      module, args_block_ptr_local, args_block_ptr, BinaryenTypeInt32());

  Ber* children = malloc((10 + args_count) * sizeof(Ber));  // TODO: 10 ??
  int children_count = 0;

  // First arg is the fn
  children[children_count++] =
      BinaryenStore(module, 4, offset, 2, local_tee_args_block_ptr, fn_arg.ber,
                    BinaryenTypeInt32());
  offset += 4;

  // Compile rest of args and add them to the args block
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;

    // Any arg that's a result of a fn call will be released
    if (result.is_fn_call) {
      /* int local_index = li_track(wasm, li, li_new(wasm)); */
      /* compiled_arg = BinaryenLocalTee(module, local_index, compiled_arg,
       */
      /*                                 BinaryenTypeInt32()); */
    }
    // Put compiled arg on args block
    Ber args_block_ptr = local_get_int32(module, args_block_ptr_local);
    children[children_count++] =
        BinaryenStore(module, 4, offset, 2, args_block_ptr, compiled_arg,
                      BinaryenTypeInt32());
    // And loop
    offset += 4;
    args = args->cdr;
  }

  WasmFn* native_fn =
      alist_get(state->wajure_to_native_fn_map, is_eq_str, "partial");
  Ber partial_fn_index = make_int32(module, native_fn->fn_table_index);
  Ber ber_args_count = make_int32(module, args_count);
  args_block_ptr = local_get_int32(module, args_block_ptr_local);
  Ber partial_operands[] = {make_int32(module, 0), make_int32(module, 0),
                            args_block_ptr, ber_args_count};

  children[children_count++] =
      BinaryenCallIndirect(module, partial_fn_index, partial_operands, 4,
                           make_type_int32(4), BinaryenTypeInt32());

  Ber block = BinaryenBlock(module, NULL, children, children_count,
                            BinaryenTypeInt32());
  /* li_close(li); */
  CResult ret = {.ber = block, .is_fn_call = 1};
  return ret;
}

CResult compile_rt_partial_call(Wasm* wasm, CResult fn_arg, Cell* args,
                                int args_count) {
  BinaryenModuleRef module = wasm->module;
  printf("================================\n");
  lval_println(fn_arg.lval);
  // Put the compiled args in a block (lalloc_size) and pass them to our
  // native partial fn
  // Check for type:
  // 1. LVAL_WASM_LAMBDA -> add partials to a copy of the lval_wasm_lambda
  // of fn and args to partial, return it
  // 2.TODO: vector/set/map -> make lval_wasm_lambda of get and args to
  // partial fn as partials
  // 3. anything else: return as is

  int extra_children = 5;
  Ber* children = malloc((args_count + extra_children) * sizeof(Ber));
  int children_count = 0;
  int wval_local = li_new(wasm);
  children[children_count++] = BinaryenLocalSet(module, wval_local, fn_arg.ber);

  Ber ber_args_count = make_int32(module, args_count - 1);

  // Calculate new partial count (partial count of fn plus count of args)
  int partial_count_local = li_new(wasm);
  Ber wval_partial_count_tee = BinaryenLocalTee(
      module, partial_count_local,
      get_wval_prop(module, local_get_int32(module, wval_local),
                    "partial_count"),
      BinaryenTypeInt32());
  Ber new_partial_count = BinaryenBinary(
      module, BinaryenAddInt32(), wval_partial_count_tee, ber_args_count);
  int new_partial_count_local = li_new(wasm);
  Ber new_partial_count_tee = BinaryenLocalTee(
      module, new_partial_count_local, new_partial_count, BinaryenTypeInt32());

  // Reserve a block of memory for the partials
  Ber lalloc_size_operands[] = {BinaryenBinary(module, BinaryenMulInt32(),
                                               new_partial_count_tee,
                                               make_int32(module, 4))};
  Ber lalloc_new_partials = BinaryenCall(
      module, "lalloc_size", lalloc_size_operands, 1, BinaryenTypeInt32());
  int new_partials_local = li_new(wasm);
  Ber set_new_partials_local =
      BinaryenLocalSet(module, new_partials_local, lalloc_new_partials);
  children[children_count++] = set_new_partials_local;

  // Copy partials of fn (first arg of partial) into this block, retaining
  // them
  Ber copy_and_retain_operands[] = {
      get_wval_prop(module, local_get_int32(module, wval_local),
                    "partials"),                     // partials of old fn
      local_get_int32(module, partial_count_local),  // old partial count
      local_get_int32(module,
                      new_partials_local)};  // ptr to partials of new fn

  WasmFn* native_fn =
      alist_get(state->wajure_to_native_fn_map, is_eq_str, "copy_and_retain");
  Ber copy_and_retain_fn_index = make_int32(module, native_fn->fn_table_index);
  Ber copy_wval_partials = BinaryenCallIndirect(
      module, copy_and_retain_fn_index, copy_and_retain_operands, 3,
      make_type_int32(3), BinaryenTypeInt32());

  int updated_partials_ptr_local = li_new(wasm);
  children[children_count++] =
      BinaryenLocalSet(module, updated_partials_ptr_local, copy_wval_partials);

  int offset = 0;
  // Copy rest of args into this block, retaining any non fn calls
  while (args) {
    CResult result = lval_compile(wasm, args->car);
    Ber compiled_arg = result.ber;

    // Any arg that's not the result of a fn call will be retained, since we
    // want to be able to release all partials when the fn is released
    if (!result.is_fn_call) wasm_retain(wasm, compiled_arg);

    // Put compiled arg on args block
    Ber partials_ptr = BinaryenLocalGet(module, updated_partials_ptr_local,
                                        BinaryenTypeInt32());
    children[children_count++] = BinaryenStore(
        module, 4, offset, 2, partials_ptr, compiled_arg, BinaryenTypeInt32());

    // And loop
    offset += 4;
    args = args->cdr;
  }

  // Copy props of fn to our partial fn
  Ber wval_fn_table_index = get_wval_prop(
      module, local_get_int32(module, wval_local), "fn_table_index");
  Ber wval_closure =
      wasm_retain(
          wasm,
          get_wval_prop(module, local_get_int32(module, wval_local), "closure"))
          .ber;

  Ber operands[4] = {
      wval_fn_table_index,
      wval_closure,
      BinaryenLocalGet(module, new_partials_local, BinaryenTypeInt32()),
      BinaryenLocalGet(module, new_partial_count_local, BinaryenTypeInt32()),
  };

  // TODO: if fn is not an function we want to return just the value itself,
  // retained, since at compile time we don't know if it's been a real fn
  // call, so we'll pretend it was, and the result of the whole partial call
  // can then be released when required

  // Make the new wval_fn
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 4, make_type_int32(1));
  children[children_count++] = make_lval_wasm_lambda_call;

  Ber block = BinaryenBlock(module, uniquify_name(wasm, "partial_call"),
                            children, children_count, BinaryenTypeInt32());

  CResult ret = {.ber = block, .is_fn_call = 1};

  return ret;
}

CResult compile_partial_call(Wasm* wasm, WasmFn native_fn, Cell* args) {
  BinaryenModuleRef module = wasm->module;
  int args_count = list_count(args);
  if (args_count == 0) quit(wasm, "Need at last one argument for partial");
  Cell* head = args;
  CResult fn_arg = lval_compile(wasm, head->car);

  head = head->cdr;
  // If just one arg, return compiled arg
  if (!head) {
    return fn_arg;
  }

  if (fn_arg.is_fn_call || fn_arg.lval->type == LVAL_REF) {
    /* return test_native_partial_fn(wasm, fn_arg, head, list_count(args)); */
    return compile_rt_partial_call(wasm, fn_arg, head, list_count(args));
  }

  switch (fn_arg.lval->type) {
    case LVAL_FUNCTION: {
      Lval* lval_fn = fn_arg.lval;
      printf("compile partial_call\n");

      // If we know this is a function at compile time then we compile the
      // rest of the args and put them in the partials block of a copy of the
      // lval_wasm_lambda we know we have.

      int fn_partial_count = list_count(lval_fn->partials);
      int total_partial_count = (fn_partial_count + (args_count - 1));

      // set_local_to_partials_ptr and make_partial_fn call
      int extra_children = 2;
      Ber* children =
          malloc(sizeof(Ber) * (total_partial_count + extra_children));
      int children_count = 0;

      // Allocate a block for the partials (including any partials of the fn
      // itself)
      Ber lalloc_size_operands[] = {
          make_int32(module, total_partial_count * 4)};
      Ber lalloc_partials = BinaryenCall(
          module, "lalloc_size", lalloc_size_operands, 1, BinaryenTypeInt32());
      int partials_ptr_index = li_new(wasm);
      Ber set_local_to_partials_ptr =
          BinaryenLocalSet(module, partials_ptr_index, lalloc_partials);
      children[children_count++] = set_local_to_partials_ptr;

      int partial_count = 0;

      // Add any partials that the fn has already
      Cell* partials_head = lval_fn->partials;
      while (partials_head) {
        Lval* lval = partials_head->car;
        CResult result = datafy_lval(wasm, lval);
        Ber wval_ptr = make_int32(module, result.data_offset);
        wval_ptr = BinaryenBinary(
            module, BinaryenAddInt32(), wval_ptr,
            BinaryenGlobalGet(module, "data_offset", BinaryenTypeInt32()));
        wval_ptr = wasm_retain(wasm, wval_ptr).ber;
        Ber partials_ptr =
            BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
        children[children_count++] =
            BinaryenStore(module, 4, partial_count++ * 4, 2, partials_ptr,
                          wval_ptr, BinaryenTypeInt32());
        partials_head = partials_head->cdr;
      }

      // Compile the args and put their lval_refs on the partials block. If
      // they're not the result of a fn call retain them.
      do {
        CResult compiled_arg = lval_compile(wasm, head->car);
        // This is so that we can release all partials when releasing the
        // lval_wasm_lambda:
        if (!compiled_arg.is_fn_call)
          compiled_arg.ber = wasm_retain(wasm, compiled_arg.ber).ber;
        Ber partials_ptr =
            BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
        children[children_count++] =
            BinaryenStore(module, 4, partial_count++ * 4, 2, partials_ptr,
                          compiled_arg.ber, BinaryenTypeInt32());
        head = head->cdr;
      } while (head);

      // Copy the props of the LVAL_FUN to our new lval_wasm_lambda:
      Ber closure_ptr = make_int32(module, 0);
      Ber partials_ptr =
          BinaryenLocalGet(module, partials_ptr_index, BinaryenTypeInt32());
      Ber ber_partial_count = make_int32(module, partial_count);

      printf("lval_fn: %d ", lval_fn->subtype);
      lval_println(lval_fn);
      Ber ber_fn_table_index;
      if (lval_fn->subtype == SYS) {
        /* printf("SYS: %s, %d\n", lval_fn->cname, lval_fn->offset); */
        ber_fn_table_index = make_int32(module, fn_arg.fn_table_index);

      } else {
        // TODO: if lval_fn is external get fn_table_index from global,
        // if it's a partial check if cfn is external
        int fn_table_index = lval_fn->cfn ? lval_fn->cfn->fn_table_index
                                          : lval_fn->fn_table_index;
        printf("offset: %d\n", fn_table_index);
        // We know the offset/fn_table_index, let's add the module's
        // fn_table_offset
        ber_fn_table_index = make_int32(module, fn_table_index);
        ber_fn_table_index = BinaryenBinary(
            module, BinaryenAddInt32(), ber_fn_table_index,
            BinaryenGlobalGet(module, "fn_table_offset", BinaryenTypeInt32()));
      }

      // Call the runtime make_lval_wasm_lambda fn
      Ber operands[4] = {ber_fn_table_index, closure_ptr, partials_ptr,
                         ber_partial_count};
      children[children_count++] =
          BinaryenCall(wasm->module, "make_lval_wasm_lambda", operands, 4,
                       make_type_int32(1));

      // And return a block that does all of the above
      Ber block = BinaryenBlock(module, uniquify_name(wasm, "make_partial_fn"),
                                children, children_count, BinaryenTypeInt32());
      free(children);
      CResult ret = {.ber = block, .is_fn_call = 1};
      return ret;
    }
    case LVAL_COLLECTION:;
      // TODO: If we know its a vector/set/map at compile time we return a
      // lval_wasm_lambda of the sys fn get, with 2+ partials: the
      // vector/set/map and the rest of the compiled args to the partial fn.
      // Falling through to default for now..
    default: {
      // If we know we have anything that's not a result of a fn call (direct
      // or lval_ref) then we return it as is , and compile the rest
      // of the args still for side effects, ie we only compile fn calls.
      Ber* children = malloc(sizeof(Ber) * args_count);  //
      int children_count = 0;
      do {
        CResult compiled_arg = lval_compile(wasm, head->car);
        // For any side effects:
        if (compiled_arg.is_fn_call) {
          compiled_arg.ber = wasm_release(wasm, compiled_arg.ber).ber;
          children[children_count++] = compiled_arg.ber;
        }
        head = head->cdr;
      } while (head);
      children[children_count++] = fn_arg.ber;

      Ber block = BinaryenBlock(
          module, uniquify_name(wasm, "make_pointless_partial_fn"), children,
          children_count, BinaryenTypeInt32());
      free(children);
      return cresult(block);
    }
  }
}
