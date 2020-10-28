#include "datafy.h"

#include "builtin.h"
#include "compile.h"
#include "inter.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "list.h"
#include "ltypes.h"
#include "lval.h"
#include "make_wasm_fn.h"
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
  WasmFn* native_fn = alist_get(state->wajure_to_native_fn_map, is_eq_str,
                                lval_fn_native->data.str);
  int lval_ptr = native_fn->data_offset;
  CResult ret = {.ber = make_int32(wasm->module, lval_ptr),
                 .data_offset = lval_ptr,
                 .fn_table_index = native_fn->fn_table_index};
  return ret;
}

CResult datafy_sys_fn(Wasm* wasm, Lval* lval_fn_sys) {
  CFn* c_fn =
      alist_get(state->wajure_to_c_fn_map, is_eq_str, lval_fn_sys->data.str);
  if (!c_fn) return datafy_native_fn(wasm, lval_fn_sys);

  int lval_ptr = c_fn->data_offset;
  CResult ret = {.ber = make_int32(wasm->module, lval_ptr),
                 .data_offset = lval_ptr,
                 .fn_table_index = c_fn->fn_table_index};
  return ret;
}

CResult datafy_root_fn(Wasm* wasm, Lval* lval_fn) {
  Lval* cfn = lval_fn->cfn;  // only partial fns have a cfn
  int offset;
  if (cfn) {
    // Add the canonical fn for this partial if we haven't already
    if (cfn->fn_table_index == -1) {
      cfn->fn_table_index = add_root_fn(wasm, cfn);
    }
    offset = cfn->fn_table_index;
  } else {
    // If not a partial fn just add the wasm fn
    lval_fn->fn_table_index = add_root_fn(wasm, lval_fn);
    offset = lval_fn->fn_table_index;
  }

  // Make a lval_wasm_lambda of our fn and inter it in wasm data
  char* data_lval =
      make_data_lval(wasm, lval_fn, wasm->__fn_table_end + offset);

  int lval_ptr = inter_data_lval(wasm, data_lval);

  // And return a wasm pointer so that it can be attached to the lval for future
  // reference
  CResult ret = {.data_offset = lval_ptr};
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
      CResult ret = {.ber = make_ptr(wasm, lval_ptr), .data_offset = lval_ptr};
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

  /* if (lval->subtype == LTRUE) { */
  /*   printf("datafy ======================= wval_ptr: %d, ",
   * lval->data_offset); */
  /*   printf("%s ", lval->ns->namespace); */
  /*   lval_println(lval); */
  /* } */

  // Every lval when it is created has its ns prop set to the namespace it was
  // found in.
  int lval_is_external = lval->ns && lval->ns != get_current_ns();
  if (lval_is_external) {
    if (lval->cname) {  // we can refer to it by name!!!
      global_name = make_global_name("data:", lval->ns->namespace, lval->cname);
      add_dep(wasm, global_name);
    } else {
      // This is from a macro, which we're compiling as if it's in this
      // namespace.

      // Alternatively we map lvals to their data offsets (and other compile
      // time info such as Context and offset) in a hash table, and lookup if we
      // datafied the lval already. And wipe this hash table every time we start
      // compiling a new namespace. We wouldn't have to record the ns on every
      // lval we make.
      lval->ns = get_current_ns();
    }
  }

  CResult ret = {};
  // A data offset in another module is of no use.
  if (!lval_is_external && lval->data_offset > 0) {
    int data_offset = lval->data_offset;
    CResult _ret = {.ber = make_ptr(wasm, data_offset),
                    .data_offset = data_offset,
                    .fn_table_index = lval->fn_table_index};
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
        int data_offset = inter_literal(wasm, lval);
        CResult _ret = {.ber = make_ptr(wasm, data_offset),
                        .data_offset = data_offset};
        ret = _ret;
      }
  }
  lval->data_offset = ret.data_offset;
  lval->fn_table_index = ret.fn_table_index;
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
