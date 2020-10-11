#include "datafy.h"

#include "compile.h"
#include "inter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "namespace.h"
#include "print.h"
#include "state.h"
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
  if (!wasm->pic) return make_int32(wasm->module, wasm->__data_end + ptr);
  Ber ber_ptr = make_int32(wasm->module, ptr);
  Ber data_offset =
      BinaryenGlobalGet(wasm->module, "data_offset", BinaryenTypeInt32());
  return BinaryenBinary(wasm->module, BinaryenAddInt32(), data_offset, ber_ptr);
}

CResult datafy_native_fn(Wasm* wasm, Lval* lval_fn_native) {
  NativeFn* native_fn = alist_get(wasm->wajure_to_native_fn_map, is_eq_str,
                                  lval_fn_native->data.str);
  if (!native_fn)
    quit(wasm, "Native function %s not found in runtime",
         lval_fn_native->data.str);

  printf("DATAFY_NATIVE_FN: %s %d\n", lval_fn_native->data.str,
         native_fn->fn_table_index);
  lval_println(lval_fn_native);
  int fn_table_index =
      0;  // not used since we're dispatching to our native fn by param_count
  int has_rest_arg = 0;  // also not used
  char* data_lval = make_data_lval(wasm, NULL, fn_table_index,
                                   native_fn->fn_table_index, has_rest_arg);
  int lval_ptr = inter_data_lval(wasm, data_lval);

  CResult ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
  return ret;
}

CResult datafy_sys_fn(Wasm* wasm, Lval* lval_fn_sys) {
  char* c_fn_name =
      alist_get(wasm->wajure_to_c_fn_map, is_eq_str, lval_fn_sys->data.str);
  if (!c_fn_name) return datafy_native_fn(wasm, lval_fn_sys);

  BinaryenModuleRef module = wasm->module;

  scoped char* fn_name = lalloc_size(512);
  _strcpy(fn_name, "sys_");
  _strcat(fn_name, lval_fn_sys->data.str);

  int fn_table_index;
  int wasm_params_count = 2;  // the function's closure and rest args
  int results_count = 1;
  BinaryenType params_type = make_type_int32(wasm_params_count);
  BinaryenType results_type = make_type_int32(results_count);
  int locals_count = 0;

  Ber stack_pointer =
      BinaryenGlobalGet(wasm->module, "stack_pointer", BinaryenTypeInt32());
  int sp_adjustment = 1 * 4;
  stack_pointer = BinaryenBinary(module, BinaryenSubInt32(), stack_pointer,
                                 make_int32(module, sp_adjustment));
  Ber rest_arg = BinaryenLocalGet(module, 1, BinaryenTypeInt32());
  Ber sys_fn_operands[2] = {make_int32(wasm->module, 0), rest_arg};

  // We pretend this fn has 1 param, namely a rest param, so any args past
  // into this fn will be wrapped in a list, ready to be passed to our sys fn.
  /* Ber process_args = wasm_process_args(wasm, 1, 1).ber; */
  // Actually call this sys fn with the rest args
  Ber call_sys_fn =
      BinaryenCall(module, c_fn_name, sys_fn_operands, 2, make_type_int32(1));
  Ber body = call_sys_fn;
  BinaryenAddFunction(module, fn_name, params_type, results_type, NULL,
                      locals_count, body);

  fn_table_index = add_fn_to_table(wasm, fn_name);
  char* data_lval =
      make_data_lval(wasm, NULL, wasm->__fn_table_end + fn_table_index, 1, 1);
  int lval_ptr = inter_data_lval(wasm, data_lval);

  CResult ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
  return ret;
}

CResult datafy_root_fn(Wasm* wasm, Lval* lval_fn) {
  Lval* cfn = lval_fn->cfn;  // only partial fns have a cfn
  int offset;
  /* Cell* partials = NULL; */
  if (cfn) {
    /* partials = lval_fn->partials; */
    // Add the canonical fn for this partial if we haven't already
    if (cfn->offset == -1) {
      add_wasm_function(wasm, cfn->closure, cfn->cname, cfn);
    }
    offset = cfn->offset;
  } else {
    // If not a partial fn just add the wasm fn
    add_wasm_function(wasm, lval_fn->closure, lval_fn->cname, lval_fn);

    offset = lval_fn->offset;
  }
  // Make a lval_wasm_lambda of our fn and inter it in wasm data
  char* data_lval =
      make_data_lval(wasm, lval_fn, wasm->__fn_table_end + offset,
                     lval_fn->param_count, lval_fn->rest_arg_index);

  int lval_ptr = inter_data_lval(wasm, data_lval);

  // And return a wasm pointer so that it can be attached to the lval for future
  // reference
  CResult ret = {.wasm_ptr = lval_ptr};
  return ret;
}

CResult datafy_collection(Wasm* wasm, Lval* lval) {
  /* printf("datafy_collection\n"); */
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
  /* lval_println(lval); */
  if (lval->type == LVAL_SYMBOL)
    return inter_lval_str_type(wasm, &wasm->lval_symbol_pool, lval);
  switch (lval->subtype) {
    case NUMBER:;
      /* lval_println(lval); */
      if (lval->data.num >= wasm->lval_num_start &&
          lval->data.num <= wasm->lval_num_end) {
        int cache =
            wasm->lval_num_offset[lval->data.num - wasm->lval_num_start];
        if (!cache) {
          cache = wasm->lval_num_offset[lval->data.num - wasm->lval_num_start] =
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
  scoped char* global_name = NULL;

  int lval_is_external = lval->ns && lval->ns != get_current_ns() ? 1 : 0;

  if (lval_is_external) {
    global_name = make_global_name("data:", lval->ns->namespace, lval->cname);
    add_dep(wasm, global_name);
  }
  /* printf("datafy ======================= (global name: %s)\n", global_name);
   */
  /* lval_println(lval); */

  CResult ret = {};
  if (!global_name && lval->wval_ptr > 0) {
    int lval_ptr = lval->wval_ptr;
    CResult _ret = {.ber = make_ptr(wasm, lval_ptr), .wasm_ptr = lval_ptr};
    /* printf("returing pre computed wval_ptr\n"); */
    return _ret;
  }
  switch (lval->type) {
    case LVAL_COLLECTION:
      if (global_name) {
        ret.ber =
            BinaryenGlobalGet(wasm->module, global_name, BinaryenTypeInt32());
      } else
        ret = datafy_collection(wasm, lval);
      break;
    case LVAL_FUNCTION:
      switch (lval->subtype) {
        case SYS:
          ret = datafy_sys_fn(wasm, lval);
          break;
        case LAMBDA:  // functions in compiler env
          if (global_name) {
            ret.ber = BinaryenGlobalGet(wasm->module, global_name,
                                        BinaryenTypeInt32());
          } else {
            ret = datafy_root_fn(wasm, lval);
          }
          break;
        case SPECIAL:
          return quit(wasm,
                      "ERROR: Can't take value of a special form such as %s",
                      lval->data.str);
        case MACRO:
          return quit(wasm, "ERROR: Can't take value of a macro such as %s",
                      global_name ? global_name : lval->data.str);
        default:;
          return quit(wasm, "ERROR: Can't compile function with subtype %d\n",
                      lval->subtype);
      }
      break;

    default:;
      if (global_name)
        ret.ber =
            BinaryenGlobalGet(wasm->module, global_name, BinaryenTypeInt32());
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
