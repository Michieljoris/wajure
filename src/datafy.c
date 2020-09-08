#include "datafy.h"

#include "compile.h"
#include "inter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "print.h"
#include "wasm.h"
#include "wasm_util.h"

//'datafying' means interring a literal lvalue. A lisp value that is not
// dependent on any variable but can be hardcoded into the program. This is done
// for strings, numbers, , nil, false, true, functions that don't close over any
// other values (other than other literal values), and lists and vectors (and
// maps and sets eventually) combining all of these. The data of these values
// (lvals) are hard coded in the wasm data section and we can just insert the
// pointer to them into the code. Wajure is immutable so these values may become
// part of any runtime created lvalues but the data itself is immutable.
// Strings, nil, false, true, numbers from -100 till 100 and function objects
// are deduped.

Ber make_ptr(Wasm* wasm, int ptr) {
  Ber ber_ptr = make_int32(wasm->module, ptr);
  if (!wasm->pic) return ber_ptr;
  Ber data_offset =
      BinaryenGlobalGet(wasm->module, "data_offset", BinaryenTypeInt32());
  return BinaryenBinary(wasm->module, BinaryenAddInt32(), data_offset, ber_ptr);
}

CResult datafy_sys_fn(Wasm* wasm, Lval* lval_sym) {
  printf("DATAFY SYS FN!!!!!\n");
  BinaryenModuleRef module = wasm->module;

  char* fn_name = lalloc_size(512);
  _strcpy(fn_name, "sys_");
  _strcat(fn_name, lval_sym->str);

  int fn_table_index;
  /* Lval* lval_num = alist_get(wasm->fn_to_offset_map, is_eq_str, fn_name); */
  /* if (lval_num) { */
  /*   fn_table_index = lval_num->num; */
  /* } else { */
  int wasm_params_count = 2;  // the function's closure and args count
  int results_count = 1;
  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(results_count);
  int locals_count = 0;
  char* c_fn_name =
      alist_get(wasm->lispy_to_c_fn_map, is_eq_str, lval_sym->str);

  if (!c_fn_name)
    quit(wasm, "System fn not found in runtime: %s", lval_sym->str);
  /* Ber rest_arg = NULL; */

  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());
  int sp_adjustment = 1 * 4;
  stack_pointer = BinaryenBinary(module, BinaryenSubInt32(), stack_pointer,
                                 make_int32(module, sp_adjustment));
  int _signed = 0;
  int _aligned = 0;
  int offset = 0;
  Ber rest_arg = BinaryenLoad(wasm->module, 4, _signed, offset, _aligned,
                              BinaryenTypeInt32(), stack_pointer);
  Ber sys_fn_operands[2] = {make_int32(wasm->module, 0), rest_arg};

  // We pretend this fn has 1 param, namely a rest param, so any args past
  // into this fn will be wrapped in a list, ready to be passed to our sys fn.
  Ber process_args = wasm_process_args(wasm, 1, 1).ber;
  // Actually call this sys fn with the rest args
  Ber call_sys_fn =
      BinaryenCall(module, c_fn_name, sys_fn_operands, 2, make_type_int32(1));
  Ber block_children[] = {process_args, call_sys_fn};
  Ber body =
      BinaryenBlock(module, "body", block_children, 2, BinaryenTypeInt32());
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL,
                      locals_count, body);

  fn_table_index = add_fn_to_table(wasm, fn_name);
  /* wasm->fn_to_offset_map = alist_prepend(wasm->fn_to_offset_map, fn_name,
   */
  /*                                        make_lval_num(fn_table_index)); */
  /* } */

  /* Ber wasm_fn_table_index = make_int32(module, fn_table_index); */
  /* Ber lispy_param_count = make_int32(module, 1);  // rest arg */
  /* Ber rest_arg_index = make_int32(module, 1); */
  /* Ber wasm_closure_pointer = make_int32(module, 0); */
  /* Ber partials = make_int32(module, 0); */
  /* Ber partial_count = make_int32(module, 0); */
  /* Ber operands[6] = {wasm_fn_table_index,  lispy_param_count, rest_arg_index,
   */
  /*                    wasm_closure_pointer, partials,          partial_count};
   */

  /* Ber make_lval_wasm_lambda_call = BinaryenCall( */
  /*     wasm->module, "make_lval_wasm_lambda", operands, 6,
   * make_type_int32(1)); */
  /* return cresult(make_lval_wasm_lambda_call); */

  /* WvalFun* wval_fun = make_lval_wasm_lambda(fn_table_index, 1, 1, 0, 0, 0);
   */
  /* printf("------------\n"); */
  /* lval_println((Lval*)wval_fun); */
  /* printf("------------\n"); */
  /* int* data_lval = (int*)wval_fun; */
  int* data_lval = make_data_lval_wasm_lambda(
      wasm, wasm->__fn_table_end + fn_table_index, 1, 1);
  int lval_ptr = inter_data_lval_wasm_lambda(wasm, data_lval);

  CResult ret = {.ber = make_ptr(wasm, lval_ptr),
                 /* make_int32(wasm->module, lval_ptr), */
                 .wasm_ptr = lval_ptr};
  return ret;

  /* printf("data_lval 0 %d %d\n", data_lval[0], LVAL_WASM_LAMBDA); */
  /* printf("data_lval 1 %d\n", data_lval[1]); */
  /* short* foo = (short*)(data_lval + 2); */

  /* printf("data_lval 2 %d\n", foo[0]); */
  /* printf("data_lval 3 %d\n", foo[1]); */
  /* printf("data_lval 4 %d\n", foo[2]); */
  /* printf("data_lval 5 %d\n", foo[3]); */
}

CResult datafy_global_lambda(Wasm* wasm, char* fn_name, Lval* lval_fun) {
  /* BinaryenModuleRef module = wasm->module; */
  fn_name = fn_name         ? fn_name
            : lval_fun->str ? lval_fun->str
                            : uniquify_name(wasm, "anon");
  /* printf("->>>>>>datafy global lambda fn %s %d!!!\n", fn_name, */
  /* lval_fun->offset); */

  /* if (lval_fun->offset == -1) { */
  FunctionData function_data =
      add_wasm_function(wasm, lval_fun->closure, fn_name, lval_fun);
  // TODO: bit sloppy adding this info to lval_fun. Should make and use
  // compiler lookup map
  /* lval_fun->offset = function_data.fn_table_index; */
  /*   lval_fun->param_count = function_data.param_count; */
  /*   lval_fun->rest_arg_index = function_data.has_rest_arg; */
  /* } */

  int* data_lval = make_data_lval_wasm_lambda(
      wasm, wasm->__fn_table_end + function_data.fn_table_index,
      function_data.param_count, function_data.has_rest_arg);
  int lval_ptr = inter_data_lval_wasm_lambda(wasm, data_lval);
  CResult ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
  return ret;
  /* Ber wasm_fn_table_index = make_int32(module, fn_table_index); */
  /* Ber lispy_param_count = make_int32(module, lval_fun->param_count); */
  /* Ber rest_arg_index = make_int32(module, lval_fun->rest_arg_index); */

  /* Ber wasm_closure_pointer = make_int32(module, 0); */
  /* Ber partials = make_int32(module, 0); */
  /* Ber partial_count = make_int32(module, 0); */

  /* Ber operands[6] = {wasm_fn_table_index,  lispy_param_count, rest_arg_index,
   */
  /*                    wasm_closure_pointer, partials,          partial_count};
   */
  /* Ber make_lval_wasm_lambda_call = BinaryenCall( */
  /*     wasm->module, "make_lval_wasm_lambda", operands, 6,
   * make_type_int32(1)); */
  /* return cresult(make_lval_wasm_lambda_call); */
}

CResult datafy_collection(Wasm* wasm, Lval* lval) {
  printf("datafy_collection\n");
  /* lval_println(lval); */
  // List, map, set, vector;
  switch (lval->subtype) {
    case LIST:
    case VECTOR:;
      int lval_ptr = inter_list(wasm, lval);
      CResult ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
      return ret;
    case MAP:;
    case SET:;
    default:
      return quit(wasm, "Unknown or unimplemented collection subtype %s %d",
                  lval_type_to_name(lval), lval->subtype);
  }
}

int inter_literal(Wasm* wasm, Lval* lval) {
  /* printf("------------------datafy_literal %s\n", lval_type_to_name(lval));
   */
  lval_println(lval);
  if (lval->type == LVAL_SYMBOL)
    return inter_lval_str_type(wasm, &wasm->lval_symbol_pool, lval);
  switch (lval->subtype) {
    case NUMBER:;
      /* lval_println(lval); */
      if (lval->num >= wasm->lval_num_start &&
          lval->num <= wasm->lval_num_end) {
        int cache = wasm->lval_num_offset[lval->num - wasm->lval_num_start];
        if (!cache) {
          cache = wasm->lval_num_offset[lval->num - wasm->lval_num_start] =
              inter_lval(wasm, lval);
        }
        return cache;
      }

      return inter_lval(wasm, lval);
    case LTRUE:
      if (!wasm->lval_true_offset)
        wasm->lval_true_offset = inter_lval(wasm, lval);
      return wasm->lval_true_offset;
    case LFALSE:
      if (!wasm->lval_false_offset)
        wasm->lval_false_offset = inter_lval(wasm, lval);
      return wasm->lval_false_offset;
    case LNIL:
      if (!wasm->lval_nil_offset)
        wasm->lval_nil_offset = inter_lval(wasm, lval);
      return wasm->lval_nil_offset;
    case STRING:
      return inter_lval_str_type(wasm, &wasm->lval_str_pool, lval);
    case KEYWORD:
      return inter_lval_str_type(wasm, &wasm->lval_keyword_pool, lval);
    default:
      return inter_lval(wasm, lval);
  }
}

CResult datafy_lval(Wasm* wasm, Lval* lval) {
  /* printf("=======================\n"); */
  /* printf("wasmfiy_lval: %d\n", lval->wval_ptr); */
  /* lval_println(lval); */
  /* printf("lval %p\n", lval); */

  CResult ret = {};
  if (lval->wval_ptr > 0) {
    /* printf("YIPPPEEEEEEE %d\n", lval->wval_ptr); */
    /* return cresult(make_int32(wasm->module, lval->wval_ptr)); */
    int lval_ptr = lval->wval_ptr;
    CResult _ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
    return _ret;
  }

  switch (lval->type) {
    case LVAL_COLLECTION:
      if (lval->global_symbol_str)
        ret.ber = BinaryenGlobalGet(wasm->module, lval->global_symbol_str,
                                    BinaryenTypeInt32());
      else
        ret = datafy_collection(wasm, lval);
      break;
    case LVAL_FUNCTION:
      switch (lval->subtype) {
        case SYS:
          ret = datafy_sys_fn(wasm, lval);
          break;
        case LAMBDA:  // functions in compiler env
          if (lval->global_symbol_str)
            ret.ber = BinaryenGlobalGet(wasm->module, lval->global_symbol_str,
                                        BinaryenTypeInt32());
          else
            ret = datafy_global_lambda(wasm, lval->str, lval);
          break;
        case SPECIAL:
          return quit(wasm,
                      "ERROR: Can't take value of a special form such as %s",
                      lval->str);
        case MACRO:
          return quit(
              wasm, "ERROR: Can't take value of a macro such as %s",
              lval->global_symbol_str ? lval->global_symbol_str : lval->str);
        default:;
          return quit(wasm, "ERROR: Can't compile function with subtype %d\n",
                      lval->subtype);
      }
      break;

    default:;
      if (lval->global_symbol_str)
        ret.ber = BinaryenGlobalGet(wasm->module, lval->global_symbol_str,
                                    BinaryenTypeInt32());
      else {
        int lval_ptr = inter_literal(wasm, lval);
        CResult _ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
        ret = _ret;
      }
  }

  lval->wval_ptr = ret.wasm_ptr;
  /* printf("lval->wval_ptr: %d!!\n", lval->wval_ptr); */
  /* printf("------------------------- \n"); */
  return ret;
}

CResult datafy_nil(Wasm* wasm) {
  Lval* lval_nil = make_lval_nil();
  Ber ret = make_ptr(wasm, inter_literal(wasm, lval_nil));
  release(lval_nil);
  return cresult(ret);
}

CResult datafy_empty_list(Wasm* wasm, Lval* lval_list) {
  if (!wasm->lval_empty_list_offset)
    wasm->lval_empty_list_offset = inter_lval(wasm, lval_list);
  return cresult(make_ptr(
      wasm, wasm->lval_empty_list_offset));  // pointer to empty lval list
}
