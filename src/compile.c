#include "compile.h"

#include <binaryen-c.h>
#include <stdlib.h>

#include "compile_special.h"
#include "env.h"
#include "fns.h"
#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "list.h"
#include "lval.h"
#include "misc_fns.h"
#include "print.h"
#include "read.h"
#include "refcount.h"
#include "special_fns.h"
#include "wasm.h"
#include "wasm_util.h"

// Builtins:
// misc: exit

Ber lval_compile(Wasm* wasm, Lval* lval);

Ber compile_fn_rest_args(Wasm* wasm, Cell* head) {
  BinaryenModuleRef module = wasm->module;
  if (!head) {
    return make_int32(module, 0);
  }
  Ber literal = lval_compile(wasm, head->car);
  Ber operands[] = {literal,
                    // recursive call:
                    compile_fn_rest_args(wasm, head->cdr)};
  return BinaryenCall(module, "list_cons", operands, 2, make_type_int32(1));
}

Ber compile_do_list(Wasm* wasm, Lval* list, int mode) {
  BinaryenModuleRef module = wasm->module;
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  int count = list_count(list->head);
  Ber* do_list = malloc(count * sizeof(Ber));  // TODO: free???

  /* int result = 0; */
  int do_list_count = 0;

  while (lval) {
    Ber literal = lval_compile(wasm, lval);
    if (do_list_count + 1 < count) literal = BinaryenDrop(module, literal);

    do_list[do_list_count++] = literal;

    /* if (result->type == LVAL_ERR) { */
    /*   lval_print(lval); */
    /*   printf(" resulted in: "); */
    /*   lval_println(result); */
    /* } */
    /* return result; */
    lval = iter_next(i);
  }

  return BinaryenBlock(module, "body", do_list, do_list_count,
                       BinaryenTypeAuto());
}

Ber wasmify_sys_fn(Wasm* wasm, Lval* lval) {
  return make_int32(wasm->module, 123);
}

Ber wasmify_root_lambda_fn(Wasm* wasm, Lval* lval) {
  return make_int32(wasm->module, 123);
}

Ber wasmify_collection(Wasm* wasm, Lval* lval) {
  return make_int32(wasm->module, 123);
}

Ber wasmify_literal(Wasm* wasm, Lval* lval) {
  switch (lval->subtype) {
    /* case STRING:; */
    /* case KEYWORD:; */
    /* case SYMBOL:; */
    case NUMBER:;
      if (lval->num >= wasm->lval_num_start &&
          lval->num <= wasm->lval_num_end) {
        if (!wasm->lval_num_offset[lval->num - wasm->lval_num_start])
          wasm->lval_num_offset[lval->num - wasm->lval_num_start] =
              make_lval_literal(wasm, lval);

        return wasm->lval_num_offset[lval->num - wasm->lval_num_start];
      }

      return make_lval_literal(wasm, lval);
    case LTRUE:
      if (!wasm->lval_true_offset)
        wasm->lval_true_offset = make_lval_literal(wasm, lval);
      return wasm->lval_true_offset;
    case LFALSE:
      if (!wasm->lval_false_offset)
        wasm->lval_false_offset = make_lval_literal(wasm, lval);
      return wasm->lval_false_offset;
    case LNIL:
      if (!wasm->lval_nil_offset)
        wasm->lval_nil_offset = make_lval_literal(wasm, lval);
      return wasm->lval_nil_offset;
    default:
      return make_lval_literal(wasm, lval);
  }
}

Ber compile_lval_compiler(Wasm* wasm, Lval* lval_symbol, Lval* lval_compiler) {
  /* Lval* lval_wasm_ref = lval_resolved_sym; */
  printf("We've got a LVAL_WASM_REF!!!\n");
  /* lval_println(lval_wasm_ref); */
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  int is_local_ref =
      context->function_context == lval_compiler->context->function_context;
  /* print_wasm_context(wasm); */
  /* print_context(lval_wasm_ref->context); */
  /* printf("------------\n"); */
  if (is_local_ref) {
    switch (lval_compiler->subtype) {
      case PARAM:;
        Ber stack_pointer = BinaryenGlobalGet(wasm->module, "stack_pointer",
                                              BinaryenTypeInt32());
        int param_count = context->function_context->param_count;

        /* printf("PPPPPPP param_count? %d\n", */
        /*        param_count); */
        int sp_adjustment = param_count * 4;
        stack_pointer =
            BinaryenBinary(module, BinaryenSubInt32(), stack_pointer,
                           make_int32(module, sp_adjustment));
        int _signed = 0;
        int _aligned = 0;

        int offset = sp_adjustment - (lval_compiler->offset + 1) * 4;
        Ber load_arg_lval =
            BinaryenLoad(wasm->module, 4, _signed, offset, _aligned,
                         BinaryenTypeInt32(), stack_pointer);
        return load_arg_lval;
      case LOCAL:;
        Ber local = BinaryenLocalGet(wasm->module, lval_compiler->offset,
                                     BinaryenTypeInt32());
        return local;
      default:
        quit(wasm, "ERROR: Unknown lval_compiler subtype: %d",
             lval_compiler->subtype);
        return NULL;
    }
  } else {
    Lval* closure_lval =
        lenv_get(context->function_context->closure, lval_symbol);
    int closure_offset;
    if (closure_lval->type == LVAL_ERR) {
      Lval* lval_closure_sym = make_lval_sym(lval_symbol->str);
      closure_offset = lval_closure_sym->offset =
          context->function_context->closure_count++;
      lenv_put(context->function_context->closure, retain(lval_symbol),
               retain(lval_compiler));
    } else {
      closure_offset = closure_lval->offset;
    }
    Ber closure_pointer = BinaryenLocalGet(
        wasm->module, 0,
        BinaryenTypeInt32());  // first param to fn is closure pointer
    int _signed = 0;
    int _aligned = 0;
    Ber load_closure_lval =
        BinaryenLoad(wasm->module, 4, _signed, closure_offset * 4, _aligned,
                     BinaryenTypeInt32(), closure_pointer);

    return load_closure_lval;
  }
}

// Resolve symbol in our compiler env and compile it. At runtime there's no
// notion of environments or symbols that resolve to other lvalues. Symbols at
// runtime are just that, a literal similar to strings, numbers etc
Ber resolve_symbol_and_compile(Wasm* wasm, Lval* lval_symbol) {
  /* printf(">>>>>>>>>>>>\n"); */
  /* printf("COMPILING SYMBOL!!! %s\n", lval_symbol->str); */
  Lval* lval_resolved_sym = lenv_get(wasm->env, lval_symbol);
  /* lval_println(lval_resolved_sym); */
  switch (lval_resolved_sym->type) {
    case LVAL_ERR:
      quit(wasm, lval_resolved_sym->str);
    case LVAL_COMPILER:
      return compile_lval_compiler(wasm, lval_symbol, lval_resolved_sym);
    case LVAL_FUNCTION:  // as evalled in our compiler env
      switch (lval_resolved_sym->subtype) {
        case SYS:
          return wasmify_sys_fn(wasm, lval_resolved_sym);
        case LAMBDA:  // root functions in compiler env
          return wasmify_root_lambda_fn(wasm, lval_resolved_sym);
        case SPECIAL:
          quit(wasm, "ERROR: Can't take value of a special form such as %s",
               lval_resolved_sym->str);
        case MACRO:
          quit(wasm, "ERROR: Can't take value of a macro such as %s",
               lval_resolved_sym->str);
        default:;
          quit(wasm, "ERROR: Can't compile function with subtype %d\n",
               lval_resolved_sym->subtype);
      }
      break;
    case LVAL_COLLECTION:
      return wasmify_collection(wasm, lval_resolved_sym);
    case LVAL_SYMBOL:
    case LVAL_LITERAL:
      return wasmify_literal(wasm, lval_resolved_sym);
  }

  /* printf("resolved sym: \n"); */
  /* lval_println(lval_resolved_sym); */

  return lval_compile(wasm, lval_resolved_sym);
}

typedef struct {
  int fn_table_index;
  Lenv* closure;
  int closure_count;
  int param_count;
  int has_rest_arg;
} FunctionData;

FunctionData add_wasm_function(Wasm* wasm, Lval* lval_sym, Lval* lval_fun) {
  /* BinaryenModuleRef module = wasm->module; */

  /* printf("fn-name: %s\n", lval_sym->str); */
  /* printf("params: "); */
  /* lval_println(lval_fun->params); */
  /* printf("body: "); */
  /* lval_println(lval_fun->body); */

  char* fn_name = lval_sym->str;

  int wasm_params_count = 2;  // the function's closure and args count
  int results_count = 1;
  CONTEXT_FUNCTION("add_wasm_function", lval_sym->str, wasm_params_count)

  // Params
  Lenv* params_env = enter_env(wasm);
  Cell* param = lval_fun->params->head;
  int lispy_param_count = 0;
  int has_rest_arg = 0;
  int i = 0;
  while (param) {
    if (has_rest_arg && lispy_param_count == has_rest_arg) {
      lval_println(lval_fun);
      quit(wasm, "ERROR: only one rest arg allowed");
    }
    Lval* lval_sym = param->car;
    if (_strcmp(lval_sym->str, "&") == 0) {
      has_rest_arg = i + 1;  // number of params
      param = param->cdr;
      continue;
    }
    lenv_put(params_env, retain(lval_sym),
             make_lval_compiler(context, PARAM, i++));
    lispy_param_count++;
    param = param->cdr;
  }
  context->function_context->param_count = lispy_param_count;

  if (has_rest_arg && lispy_param_count != has_rest_arg) {
    quit(wasm, "ERROR: rest arg missing");
  }

  // Compile body
  Ber body = compile_do_list(wasm, lval_fun->body, 0);
  printf("Done compile_do_list\n");

  // Post compile
  leave_env(wasm);

  /* printf("local_count: %d\n", *context->local_count); */
  int locals_count = context->function_context->local_count - wasm_params_count;

  lval_fun->offset = add_fn_to_table(wasm, fn_name);

  BinaryenModuleRef module = wasm->module;

  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(results_count);

  BinaryenType* local_types = make_type_int32_array(locals_count);

  // Add fn to wasm
  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, body);

  FunctionData function_data = {lval_fun->offset,
                                retain(context->function_context->closure),
                                context->function_context->closure_count,
                                lispy_param_count, has_rest_arg};
  return function_data;
}

Ber wasm_lalloc_type(Wasm* wasm, int type) {
  Ber operands[1] = {make_int32(wasm->module, type)};
  return BinaryenCall(wasm->module, "lalloc_type", operands, 1,
                      make_type_int32(1));
}

Ber wasm_lalloc_size(Wasm* wasm, int size) {
  Ber operands[1] = {make_int32(wasm->module, size)};
  return BinaryenCall(wasm->module, "lalloc_size", operands, 1,
                      make_type_int32(1));
}

Ber compile_lambda(Wasm* wasm, Lval* lval_list) {
  BinaryenModuleRef module = wasm->module;

  Context* context = wasm->context->car;
  Lval* arg_list = new_lval_list(retain(lval_list->head->cdr));
  Lval* lval_fun = eval_lambda_form(NULL, arg_list, LAMBDA);
  release(arg_list);
  lval_println(lval_fun);
  Lval* lval_sym = make_lval_sym(NULL);
  char* lambda_name = lalloc_size(512);
  _strcpy(lambda_name, context->function_context->fn_name);
  _strcat(lambda_name, "_");
  itostr(lambda_name + _strlen(lambda_name), wasm->fns_count + 1);
  lambda_name = lrealloc(lambda_name, _strlen(lambda_name) + 1);
  lval_sym->str = retain(lambda_name);

  FunctionData function_data = add_wasm_function(wasm, lval_sym, lval_fun);

  printf("COMPILED FUNCTION!!! %d %d\n", function_data.fn_table_index,
         function_data.closure_count);

  release(lval_sym);
  release(lval_fun);

  Ber fn_table_index = make_int32(module, function_data.fn_table_index);
  Ber partial_count = make_int32(module, 0);
  Ber partials = make_int32(module, 0);  // NULL
  Ber lispy_param_count = make_int32(module, function_data.param_count);
  Ber wasm_has_rest_arg = make_int32(module, function_data.has_rest_arg);

  Ber* lambda_list =
      malloc((function_data.closure_count + 2) * sizeof(Ber));  // TODO: free???

  /* env_print(function_data.closure); */
  printf("closure from compiled fn:\n");
  Cell* closure_cell = function_data.closure->kv;
  int lambda_block_count = 0;

  // Allocate a block of memory to hold the closure for the lambda, and store
  // pointer in local var
  Ber wasm_closure_pointer =
      wasm_lalloc_size(wasm, function_data.closure_count * 4);
  int closure_pointer_local_index = context->function_context->local_count;
  Ber local_set = BinaryenLocalSet(module, closure_pointer_local_index,
                                   wasm_closure_pointer);
  context->function_context->local_count++;
  lambda_list[lambda_block_count++] = local_set;

  wasm_closure_pointer = BinaryenLocalGet(module, closure_pointer_local_index,
                                          BinaryenTypeInt32());

  // Store lval pointers in the closure memory block
  int offset = function_data.closure_count * 4;
  while (closure_cell) {
    Cell* pair = closure_cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;
    Ber closure_lval_pointer = compile_lval_compiler(wasm, lval_sym, lval);
    offset -= 4;

    Ber store_wasm_ref =
        BinaryenStore(module, 4, offset, 0, wasm_closure_pointer,
                      closure_lval_pointer, BinaryenTypeInt32());
    lambda_list[lambda_block_count++] = store_wasm_ref;
    /* lval_print(lval_sym); */
    /* printf(": %d ", lval->offset); */
    /* lval_println(lval); */
    closure_cell = closure_cell->cdr;
  }
  /* printf("Current env:\n"); */
  /* env_print(wasm->env); */

  /* printf("Current closure:\n"); */
  /* env_print(context->function_context->closure); */

  // Make a lval_wasm_lambda with info on fn table index, param count, pointer
  // to closure etc.
  Ber operands[6] = {fn_table_index,       lispy_param_count, wasm_has_rest_arg,
                     wasm_closure_pointer, partials,          partial_count};
  Ber make_lval_wasm_lambda_call = BinaryenCall(
      wasm->module, "make_lval_wasm_lambda", operands, 6, make_type_int32(1));

  lambda_list[lambda_block_count++] = make_lval_wasm_lambda_call;

  Ber lambda_block = BinaryenBlock(module, lambda_name, lambda_list,
                                   lambda_block_count, BinaryenTypeAuto());
  release(lambda_name);
  return lambda_block;
}

Ber compile_special_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile special call!!!!!!!!!!!!!!!\n"); */
  char* fn_name = lval_fn_sym->str;
  if (_strcmp(fn_name, "let") == 0) return compile_let(wasm, args);
  if (_strcmp(fn_name, "if") == 0) return compile_if(wasm, args);
  if (_strcmp(fn_name, "try") == 0) return compile_try(wasm, args);
  if (_strcmp(fn_name, "throw") == 0) return compile_throw(wasm, args);
  if (_strcmp(fn_name, "quote") == 0) return compile_quote(wasm, args);
  if (_strcmp(fn_name, "quasiquote") == 0)
    return compile_quasiquote(wasm, args);
  quit(wasm, "ERROR: Unknown special form: %s", fn_name);
  return NULL;
}

int is_eq_str(void* k1, void* k2) { return _strcmp(k1, k2) == 0; }

Ber compile_sys_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  /* printf("compile sys call!!!!!!!!!!!!!!!\n"); */
  Ber arg_list_head = compile_fn_rest_args(wasm, args);

  Ber lval_list_operands[1] = {arg_list_head};
  Ber wasm_lval_list = BinaryenCall(wasm->module, "new_lval_list",
                                    lval_list_operands, 1, make_type_int32(1));

  Ber sys_fn_operands[2] = {make_int32(wasm->module, 0), wasm_lval_list};
  char* c_fn_name =
      alist_get(wasm->lispy_to_c_fn_map, is_eq_str, lval_fn_sym->str);

  if (!c_fn_name)
    quit(wasm, "System fn not found in runtime: %s", lval_fn_sym->str);

  Ber sys_fn_call = BinaryenCall(wasm->module, c_fn_name, sys_fn_operands, 2,
                                 make_type_int32(1));

  return sys_fn_call;
}

Ber compile_wasm_fn_call(Wasm* wasm, Ber lval_wasm_ref, char* fn_name,
                         Cell* args) {
  BinaryenModuleRef module = wasm->module;
  Context* context = wasm->context->car;
  printf("compile local ref call!!!!!!!!!!!!!!!\n");
  // Puts args of a fn like (f a b c) on stack like this: <c b a 3> where stack
  // pointer points at <
  int i = 0;
  int args_count = list_count(args);
  int wasm_args_count = args_count + 4;  // args,  update stackpointer,
                                         // call, reset stackpointer, result
  /* wasm_args_count++; //for wval_print */
  Ber* wasm_args = malloc(sizeof(Ber) * wasm_args_count);
  int sp_offset = args_count * 4;
  printf("SP_OFFSET: %d\n", sp_offset);
  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());

  // Put args_count on stack
  /* Ber push_args_count = */
  /*     BinaryenStore(module, 4, 0, 0, stack_pointer, */
  /*                   make_int32(module, args_count), BinaryenTypeInt32()); */
  /* wasm_args[i++] = push_args_count; */

  // Put args on stack
  while (args) {
    Ber push_arg =
        BinaryenStore(module, 4, sp_offset - (i + 1) * 4, 0, stack_pointer,
                      lval_compile(wasm, args->car), BinaryenTypeInt32());
    wasm_args[i++] = push_arg;
    args = args->cdr;
  }

  // Increase stack pointer to free mem.
  BinaryenExpressionRef sp_plus_offset = BinaryenBinary(
      module, BinaryenAddInt32(), stack_pointer, make_int32(module, sp_offset));
  wasm_args[i++] = BinaryenGlobalSet(module, "stack_pointer", sp_plus_offset);

  // Call fn
  /* BinaryenType params_type = make_type_int32(1); */
  /* BinaryenType results_type = make_type_int32(1); */
  // TODO: write in wasm:
  // see if lval_wasm_ref === LVAL_WASM_LAMBDA
  // if so call indirect, get fn_index and closure pointer first
  // else print "unimplemented!!!" and leave 123 on stack.
  // Call fn

  Ber result = NULL;
  if (lval_wasm_ref) {
    // get fn_index and closure pointer from lval_wasm_ref
    Ber wasm_fn_index =
        BinaryenLoad(module, 2, 0, 2, 0, BinaryenTypeInt32(), lval_wasm_ref);
    /* wasm_fn_index = make_int32(module, 1); */
    Ber wasm_closure_pointer =
        BinaryenLoad(module, 4, 0, 12, 0, BinaryenTypeInt32(), lval_wasm_ref);
    Ber operands[] = {wasm_closure_pointer, make_int32(module, args_count)};
    BinaryenType params_type = make_type_int32(2);

    /* Ber wval_operands[] = {lval_wasm_ref}; */
    /* Ber wval_print = BinaryenCall(module, "wval_print", wval_operands, 1, */
    /*                               BinaryenTypeNone()); */
    /* wasm_args[i++] = wval_print; */

    result = BinaryenCallIndirect(module, wasm_fn_index, operands, 2,
                                  params_type, BinaryenTypeInt32());
  }

  if (fn_name) {
    Ber operands[] = {make_int32(module, 0), make_int32(module, args_count)};
    result = BinaryenCall(module, fn_name, operands, 2, BinaryenTypeInt32());
  }

  // Store result in local var
  int result_index = context->function_context->local_count++;
  wasm_args[i++] = BinaryenLocalSet(module, result_index, result);

  // Reset stack pointer;
  BinaryenExpressionRef sp_minus_offset = BinaryenBinary(
      module, BinaryenSubInt32(), stack_pointer, make_int32(module, sp_offset));
  wasm_args[i++] = BinaryenGlobalSet(module, "stack_pointer", sp_minus_offset);

  // Put result on stack
  Ber result_from_local =
      BinaryenLocalGet(module, result_index, BinaryenTypeInt32());
  wasm_args[i++] = result_from_local;

  Ber call_block = BinaryenBlock(module, "lambda_call", wasm_args,
                                 wasm_args_count, BinaryenTypeInt32());
  return call_block;

  // Write wasm that does the following:
  // Compile the args to wasm, put them on lispy stack in reverse order, with
  // count of args on top slot.

  // Check whether the local ref is a lambda (which might be a wrapper for sys
  // and named fns)
  //
  // Write fns to deal with keyword, vector, set and map, dispatch by type and
  // pass first node plus args to fn

  // It's the callee's responsibility to check correct number of args (so
  // could be disabled by flag). Without flag it's possible to call a fn with
  // too few args, which would have unpredictable results. Too many doesn't
  // hurt necessarily. It's also callee's responsibility to wrap up excess
  // args into a rest arg. Otherwise we would have to check for this for every
  // fn call. Throw error if count of args < param count (without any rest
  // arg): [a b] has param count of 2, but [a b & c] has param count of 2 as
  // well. If fn has rest arg then insert code that gathers excess args up
  // into list and stores that list in a local var to be referred to in the
  // rest of the fn. Only insert param checking code when lispy compiler flag
  // is set for it. Same for rest arg, only insert wasm code for it when
  // appropriate.

  // When a LVAL_WASM_LAMBDA is a result of a partial its fn_table_index
  // points to a wrapper fn that puts the partial args after the current stack
  // pointer and increases sp by partial_count, and then calls the actual fn
  // (which again might be a partial).

  // Call indirect fn with the right offset, and one arg (pointer to closure)
  // Set stack pointer to free mem; (so add param_count + 1  to sp, last slot
  // holds param_count itself).
  // On return from this call set stack pointer back.
}

Ber compile_root_fn_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  return compile_wasm_fn_call(wasm, NULL, lval_fn_sym->str, args);
}

Ber compile_list(Wasm* wasm, Lval* lval_list);

Ber compile_as_fn_call(Wasm* wasm, Lval* lval, Cell* args) {
  switch (lval->type) {
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:;
          Ber compiled_list = compile_list(wasm, lval);
          return compile_wasm_fn_call(wasm, compiled_list, NULL, args);
        case VECTOR:
        case MAP:
        case SET:
          quit(wasm, "ERROR: Map/set/vector cannot be cast as fn (yet)\n");
        default:
          quit(wasm, "ERROR: Unknown collectiion subtype %d", lval->subtype);
      }
      break;
    case LVAL_LITERAL:
      quit(wasm,
           "ERROR: Literal or symbol cannot be cast as fn (not even a "
           "keyword, "
           "yet)\n");
    case LVAL_ERR:
      quit(wasm, "ERROR compile_list: there was an error in parsing code: %s\n",
           lval->str);
    default:
      quit(wasm, "ERROR compile_list: Unknown lval type %d\n", lval->type);
  }
  return NULL;
}

Ber compile_list(Wasm* wasm, Lval* lval_list) {
  /* BinaryenModuleRef module = wasm->module; */
  CONTEXT_LVAL("compile_list", lval_list);

  // Empty list
  if (lval_list->head == NIL) {
    if (!wasm->lval_empty_list_offset)
      wasm->lval_empty_list_offset = make_lval_literal(wasm, lval_list);
    return wasm->lval_empty_list_offset;  // pointer to empty lval list
  }

  Lval* lval_first = lval_list->head->car;

  // Lambda
  if (_strcmp(lval_first->str, "fn") == 0) {
    return compile_lambda(wasm, lval_list);
  }

  Cell* args = lval_list->head->cdr;

  // Fn call > (symbol args ...)
  if (lval_first->type == LVAL_SYMBOL) {
    printf("resolving symbol %s\n", lval_first->str);
    // Let's see if the symbol refers to something we know how to compile
    // already
    Lval* resolved_symbol = lenv_get(wasm->env, lval_first);
    // If it's a symbol it has to be known in our compiler env!!!
    if (resolved_symbol->type == LVAL_ERR) {
      lval_println(lval_list);
      lval_println(lval_first);
      quit(wasm, "ERROR: Unknowm symbol while trying to compile a list %s",
           lval_first->str);
    }

    switch (resolved_symbol->type) {
      case LVAL_FUNCTION:  // as evalled in our compiler env
        switch (resolved_symbol->subtype) {
          case SYS:
            return compile_sys_call(wasm, lval_first, args);
          case SPECIAL:
            return compile_special_call(wasm, lval_first, args);
          case LAMBDA:  // root functions in compiler env
            return compile_root_fn_call(wasm, lval_first, args);
          case MACRO:
            // TODO: macroexpand-all on this sucker!!
            quit(wasm, "ERROR: Can't compile macro!!");
          default:
            /* lval_println(resolved_symbol); */
            quit(wasm,
                 "ERROR: Can't compile function with unknown subtype %d\n",
                 lval_first->subtype);
        }
      case LVAL_COMPILER:;
        // we've assigned something to a local ref in wasm, but we don't know
        // what it is, and whether it's a fn even. It'll have to be worked out
        // at runtime. With some clever optimisations we can in some cases
        // probably work out what it is at compile time and can reduce it to a
        // compile_list/lambda_cal. However the default case is that we cannot
        // assume anything about the lval.
        Ber compiled_symbol = resolve_symbol_and_compile(wasm, lval_first);
        return compile_wasm_fn_call(wasm, compiled_symbol, NULL, args);
      case LVAL_SYMBOL:
        quit(wasm, "ERROR: A symbol can't ve cast to a fn: %s",
             resolved_symbol->str);
      case LVAL_COLLECTION:
        switch (resolved_symbol->subtype) {
          case LIST:
            quit(wasm, "ERROR: A list can't be cast to a fn");
          default:;
        }
      default:
        // Not a function or wasm ref, perhaps it's something else we can use
        // as a fn
        return compile_as_fn_call(wasm, resolved_symbol, args);
    }
  } else
    // Not a function or wasm ref, perhaps it's something else we can use as a
    // fn
    return compile_as_fn_call(wasm, lval_first, args);

  quit(wasm, NULL);
  exit(1);
}

// This will compile an unevalled lval (so which is composed of only literals,
// symbols and collections) to wasm code that will leave a pointer to an lval
// on the stack. This time the lval can also be a lambda. Eg a symbol might
// resolve to something we know is a function (like a sys or a root fn), or a
// list is of the form (fn [..] ..).
Ber lval_compile(Wasm* wasm, Lval* lval) {
  /* printf("lval_compile: "); */
  /* lval_println(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */

  switch (lval->type) {
    case LVAL_SYMBOL:
      return resolve_symbol_and_compile(wasm, lval);
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          return compile_list(wasm, lval);  // fn call
        case SET:
        case MAP:
        case VECTOR:
          return wasmify_collection(wasm, lval);
        default:;
          quit(wasm, "ERROR: unknown collection subtype: %d", lval->subtype);
      }
    case LVAL_LITERAL:
      return wasmify_literal(wasm, lval);
    default:;
      quit(wasm, "ERROR: Unknown lval type: %d", lval->type);
  }
  return NULL;
}

void print_pair(Lval* lval_sym, Lval* lval) {
  lval_print(lval_sym);
  printf(": ");
  lval_print(lval);
}

int compile(char* file_name) {
  info("Compiling %s\n", file_name);
  /* print_slot_size(); */

  set_log_level(LOG_LEVEL_INFO);

  Wasm* wasm = init_wasm();
  /* printf("__data_end: %d\n", wasm->__data_end); */
  wasm->env = interprete_file(file_name);

  import_runtime(wasm);

  printf("Processing env ==============================\n");
  Cell* cell = wasm->env->kv;
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    /* print_pair(lval_sym, lval); */
    if (lval->type == LVAL_FUNCTION) {
      add_wasm_function(wasm, lval_sym, lval);
    }

    cell = cell->cdr;
    /* if (cell) printf("%s", "\n"); */
    /* printf("\n"); */
  }

  BinaryenAddFunctionExport(wasm->module, "test", "test");

  add_fn_to_table(wasm, "log_int");
  add_fn_to_table(wasm, "printf_");
  // We only have this data _after_ compiling
  /* add_global_section(module); */
  add_function_table(wasm);

  /* add_string_to_data(wasm, "foo3"); */
  add_memory_section(wasm);

  int validate_result = BinaryenModuleValidate(wasm->module);
  if (!validate_result)
    printf("VALIDATE ERROR!!!!!!\n");
  else
    printf("VALIDATED OK!!\n");
  /* BinaryenModulePrint(wasm->module); */

  write_wat(wasm, "compiled/lispy.wat");
  write_wasm(wasm, "compiled/lispy.wasm");

  free_wasm(wasm);

  return 0;
}

/* Lval* parse_file(char* file_name) { */
/*   char* str = read_file(file_name); */
/*   if (!str) { */
/*     printf("Could not load file %s", str); */
/*     return NULL; */
/*   } */

/*   int pos = 0; */
/*   Lval* lval_list = lval_read_list(str, &pos, '\0'); */
/*   if (lval_list->type == LVAL_ERR) { */
/*     lval_println(lval_list); */
/*     return NULL; */
/*   } */
/*   return lval_list; */
/* } */

/* void add_global_section(BinaryenModuleRef module) { */
/*   BinaryenAddGlobalImport(module, "data_end", "env", "data_end", */
/*                           BinaryenTypeInt32(), 0); */

/*   BinaryenAddGlobal(module, "a-global", BinaryenTypeInt32(), 0, */
/*                     make_int32(module, 7)); */

/*   BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(), 1,
 */
/*                     make_int32(module, 123)); */
/* } */

/* BER call_indirect_example(Wasm* wasm) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   BER fn_index = make_int32(module, 0); */
/*   BER operands[] = {wasm_offset(wasm, 5), */
/*                                       make_int32(module, 0)}; */

/*   BinaryenType params_type = make_type_int32(2); */

/*   BinaryenType results_type = make_type_int32(0); */

/*   BER call_indirect = BinaryenCallIndirect( */
/*       module, fn_index, operands, 2, params_type, results_type); */

/*   BER drop = BinaryenDrop(module, call_indirect); */
/*   return drop; */
/* } */

/* struct table_fn { */
/*   char* fn_name; */
/*   int table_index; */
/*   int operands_count; */
/*   BinaryenType operands_type; */
/*   int results_count; */
/* }; */

/* add_string_to_data(wasm, "foo3"); */
/* add_string_to_data(wasm, "bar"); */

/* BER compile_symbol_literal(Wasm* wasm, Lval* lval) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   int len = _strlen(lval->str); */
/*   BER wasm_len = make_int32(module, len); */
/*   BER operands_lalloc[1] = {wasm_len}; */
/*   BER lalloc_size = BinaryenCall( */
/*       module, "lalloc_size", operands_lalloc, 1, make_type_int32(1)); */

/*   // TODO: see if str already exists and return that offset instead of
 * adding
 */
/*   // str again to data */
/*   int offset = add_string_to_data(wasm, lval->str); */

/*   BER operands_strcpy[2] = {lalloc_size, */
/*                                               wasm_offset(wasm, offset)};
 */
/*   BER _strcpy = */
/*       BinaryenCall(module, "_strcpy", operands_strcpy, 2,
 * make_type_int32(1)); */
/*   BER operands[1] = {_strcpy}; */

/*   return BinaryenCall(module, "make_lval_sym", operands, 1,
 * make_type_int32(1)); */
/* } */

/* BER compile_lval_literal(Wasm* wasm, Lval* lval) { */
/*   BinaryenModuleRef module = wasm->module; */
/*   /\* printf("lval_compile literal: "); *\/ */
/*   BER* operands = NULL; */
/*   char* func_name; */
/*   int operands_count = 1; */
/*   switch (lval->subtype) { */
/*     case NUMBER:; */
/*     case STRING:; */
/*       return compile_lval_literal(wasm, lval); */
/*       /\* int len = _strlen(lval->str); *\/ */
/*       /\* BER wasm_len = make_int32(module, len); *\/ */
/*       /\* BER operands_lalloc[1] = {wasm_len}; *\/ */
/*       /\* BER lalloc_size = BinaryenCall( *\/ */
/*       /\*     module, "lalloc_size", operands_lalloc, 1,
 * make_type_int32(1));
 * *\/ */

/*       /\* int offset = add_string_to_data(wasm, lval->str); *\/ */

/*       /\* BER operands_strcpy[2] = {lalloc_size, *\/ */
/*       /\*                                             wasm_offset(wasm,
 * offset)}; */
/*        *\/ */
/*       /\* BER _strcpy = BinaryenCall( *\/ */
/*       /\*     module, "_strcpy", operands_strcpy, 2, make_type_int32(1));
 * *\/
 */
/*       /\* BER operands_string[1] = {_strcpy}; *\/ */

/*       /\* operands = operands_string; *\/ */
/*       /\* func_name = "make_lval_str"; *\/ */
/*       /\* break; *\/ */
/*     case LNIL: */
/*       operands_count = 0; */
/*       func_name = "make_lval_nil"; */
/*       break; */
/*     case LTRUE: */
/*       operands_count = 0; */
/*       func_name = "make_lval_true"; */
/*       break; */
/*     case LFALSE: */
/*       operands_count = 0; */
/*       func_name = "make_lval_false"; */
/*       break; */
/*     default: */
/*       return make_int32(module, 123); */
/*   } */

/*   BER literal = BinaryenCall( */
/*       module, func_name, operands, operands_count, make_type_int32(1)); */

/*   return literal; */
/* } */

/* printf("offset: %d\n", wasm->strings_offset); */
/* /\* printf("S2 %s:\n", s2); *\/ */
/* char* s = (char*)data_lval; */

/* for (int i = 0; i < 14; i++) printf("%x,", data_lval[i]); */
/* printf("\n"); */
/* for (int i = 0; i < 36; i++) { */
/*   if (i % 4 == 0) printf("|"); */
/*   printf("%x,", s[i]); */
/* } */

/* BER BinaryenCall(BinaryenModuleRef module, */
/*                                    const char* target, */
/*                                    BER* operands, */
/*                                    BinaryenIndex numOperands, */
/*                                    BinaryenType returnType); */

/* BINARYEN_API BER */
/* BinaryenCallIndirect(BinaryenModuleRef module, */
/*                      BER target, */
/*                      BER* operands, */
/*                      BinaryenIndex numOperands, */
/*                      BinaryenType params, */
/*                      BinaryenType results); */

/* int __data_end; */

/* compile_fn_rest_args(wasm, lval->body->head); */
/* BER drop = call_indirect_example(wasm); */

/* BER lval_list_operands[1] = {arg_list_head}; */
/* BER lval_list = BinaryenCall( */
/*     module, "new_lval_list", lval_list_operands, 1, make_type_int32(1));
 */

/* BER lval_println_operands[1] = {lval_list}; */
/* BER lval_println_operands[1] = {wasm_offset(wasm, 22)};
 */
/* BER body = BinaryenCall( */
/*     module, "lval_println", lval_println_operands, 1,
 * BinaryenTypeNone());
 */

/* BER print_slot_size = */
/*     BinaryenCall(module, "print_slot_size", NULL, 0, BinaryenTypeNone());
 */
/* body = print_slot_size; */

/* printf("fn-name: %s\n", lval_sym->str); */
/* printf("params: "); */
/* lval_println(lval->params); */
/* printf("body: "); */
/* lval_println(lval->body); */

// Load: align can be 0, in which case it will be the natural alignment (equal
// to bytes)
/* BINARYEN_API BinaryenExpressionRef BinaryenLoad(BinaryenModuleRef module,
 */
/*                                                 uint32_t bytes, */
/*                                                 int8_t signed_, */
/*                                                 uint32_t offset, */
/*                                                 uint32_t align, */
/*                                                 BinaryenType type, */
/*                                                 BinaryenExpressionRef ptr);
 */
/* // Store: align can be 0, in which case it will be the natural alignment
 * (equal */
/* // to bytes) */
/* BINARYEN_API BinaryenExpressionRef BinaryenStore(BinaryenModuleRef module,
 */
/*                                                  uint32_t bytes, */
/*                                                  uint32_t offset, */
/*                                                  uint32_t align, */
/*                                                  BinaryenExpressionRef ptr,
 */
/*                                                  BinaryenExpressionRef
 * value,
 */
/*                                                  BinaryenType type); */

/* void add_wasm_function(Wasm* wasm, char* fn_name, int params_count, */
/*                        int locals_count, Ber body, int results_count) { */
/*   BinaryenModuleRef module = wasm->module; */

/*   BinaryenType params_type = make_type_int32(params_count); */
/*   BinaryenType results_type = make_type_int32(results_count); */
/*   /\* BinaryenType results_type = make_results_type(results_count); *\/ */

/*   BinaryenType* local_types = make_type_int32_array(locals_count); */

/*   BinaryenAddFunction(module, fn_name, params_type, results_type,
 * local_types, */
/*                       locals_count, body); */
/* } */

/* TODO: */
//
// call root fn
// check whether wasm ref is a a lambda/set/map/vector, print error msg if not
// wasmify sys and root fns
