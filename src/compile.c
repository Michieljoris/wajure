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

enum { RETURN_ON_ERROR, RETURN_LAST_EVAL };

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

Ber compile_symbol(Wasm* wasm, Lval* lval_symbol) {
  printf("COMPILING SYMBOL!!! %s\n", lval_symbol->str);
  Lval* lval_resolved_sym = lenv_get(wasm->env, lval_symbol);
  if (lval_resolved_sym->type == LVAL_ERR) {
    quit(wasm, lval_resolved_sym->str);
  }

  /* printf("resolved sym: \n"); */
  /* lval_println(lval_resolved_sym); */

  return lval_compile(wasm, lval_resolved_sym);
}

Ber compile_special_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  printf("compile special call!!!!!!!!!!!!!!!\n");
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

Ber compile_lambda_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  printf("compile lambda call!!!!!!!!!!!!!!!\n");
  return make_int32(wasm->module, 123);
}

Ber compile_local_ref_call(Wasm* wasm, Lval* lval_fn_sym, Cell* args) {
  printf("compile local ref call!!!!!!!!!!!!!!!\n");
  return make_int32(wasm->module, 123);
}

void _leave_context(void* data) { leave_context(*(void**)data); }

int compile_function(Wasm* wasm, Lval* lval_sym, Lval* lval);

Ber compile_fn_call(Wasm* wasm, Lval* lval_list) {
  /* BinaryenModuleRef module = wasm->module; */
  CONTEXT_LVAL("compile_fn_call", lval_list);

  if (lval_list->head == NIL) {
    if (!wasm->lval_empty_list_offset)
      wasm->lval_empty_list_offset = make_lval_literal(wasm, lval_list);
    return wasm->lval_empty_list_offset;
  }

  // TODO: add params of fn to env!!

  Lval* lval_first = lval_list->head->car;

  if (_strcmp(lval_first->str, "fn") == 0) {
    Lval* arg_list = new_lval_list(retain(lval_list->head->cdr));
    Lval* lval_fun = eval_lambda_form(NULL, arg_list, LAMBDA);
    release(arg_list);
    lval_println(lval_fun);
    Lval* lval_sym = make_lval_sym(NULL);
    char* lambda_name = lalloc_size(512);
    _strcpy(lambda_name, context->fn_name);
    _strcat(lambda_name, "_");
    itostr(lambda_name + _strlen(lambda_name), wasm->fns_count + 1);
    lambda_name = lrealloc(lambda_name, _strlen(lambda_name) + 1);
    lval_sym->str = lambda_name;
    int fn_offset = compile_function(wasm, lval_sym, lval_fun);
    release(lval_sym);
    release(lval_fun);
    scoped Lval* lval_local_ref = make_lval_local_ref(LAMBDA, fn_offset);
    return make_lval_literal(wasm, lval_local_ref);
    /* Ber local_ref_fn = make_lval_listeral(wasm, lval_local_ref); */
    /* release(lval_local_ref); */
    /* return make_int32(wasm->module, fn_offset); */
  }

  if (lval_first->type == LVAL_SYMBOL) {
    lval_first = lenv_get(wasm->env, lval_first);
    // If it's a symbol it has to be known in our compiler env!!!
    if (lval_first->type == LVAL_ERR) {
      lval_println(lval_list);
      lval_println(lval_first);
      quit(wasm, NULL);
    }
  }

  Cell* args = lval_list->head->cdr;
  switch (lval_first->type) {
    case LVAL_FUNCTION:  // as evalled in our compiler env
      switch (lval_first->subtype) {
        case SYS:
          return compile_sys_call(wasm, lval_first, args);
        case SPECIAL:
          // TODO: don't allow special symbols to be assigned to another
          // symbol!!!!
          return compile_special_call(wasm, lval_first, args);
        case LAMBDA:
          return compile_lambda_call(wasm, lval_first, args);
        default:
          printf("ERROR: Can't compile function with subtype %d\n",
                 lval_first->subtype);
          lval_println(lval_list);
      }
      break;
    case LVAL_LOCAL_REF:
      // we've assigned something to a local ref in wasm, but we don't know
      // what it is, and whether it's a fn even. It'll have to be worked out
      // at runtime. With some clever optimisations we can in some cases
      // probably work out what it is at compile time and can reduce it to a
      // compile_fn/lambda_cal. However the default case is that we cannot
      // assume anything about the lval.
      return compile_local_ref_call(wasm, lval_first, args);
      break;
    case LVAL_COLLECTION:
      switch (lval_first->subtype) {
        case LIST:
          // TODO: ????? this might or might not return a fn at runtime, so we
          // need to a compile_local_ref_call on the result of this somehow
          return compile_fn_call(wasm, lval_first);
        case VECTOR:
        case MAP:
        case SET:
          lval_println(lval_list);
          printf("ERROR: Map/set/vector cannot be cast as fn (yet)\n");
          break;
        default:
          lval_println(lval_list);
          printf("ERROR: Unknown collectiion subtype %d", lval_first->subtype);
      }
      break;
    case LVAL_SYMBOL:
    case LVAL_LITERAL:
      lval_println(lval_list);
      printf(
          "ERROR: Literal or symbol cannot be cast as fn (not even a keyword, "
          "yet)\n");
      break;
    case LVAL_ERR:
      lval_println(lval_first);
      lval_println(lval_list);
      printf("ERROR compile_fn_call: there was an error in parsing code\n");
      break;
    default:
      lval_println(lval_list);
      printf("ERROR compile_fn_call: Unknown lval type %d\n", lval_first->type);
  }

  quit(wasm, NULL);
  exit(1);
}

Ber compile_vector(Wasm* wasm, Lval* lval) {
  BinaryenModuleRef module = wasm->module;
  return make_int32(module, 0);
}

Ber compile_map(Wasm* wasm, Lval* lval) {
  BinaryenModuleRef module = wasm->module;
  return make_int32(module, 0);
}

Ber compile_lval_literal(Wasm* wasm, Lval* lval) {
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

Ber lval_compile(Wasm* wasm, Lval* lval) {
  printf("lval_compile: ");
  lval_println(lval);
  printf("lval->type: %s\n", lval_type_constant_to_name(lval->type));
  /* BER ref = NULL; */
  switch (lval->type) {
    case LVAL_SYMBOL:
      return compile_symbol(wasm, lval);
    case LVAL_COLLECTION:
      switch (lval->subtype) {
        case LIST:
          return compile_fn_call(wasm, lval);
          break;
        case VECTOR:
          return compile_vector(wasm, lval);
          break;
        case MAP:
          /* TODO: */
          return compile_map(wasm, lval);
          break;
        default:;
          /* return make_err("Unknown seq subtype: %d ", lval->subtype); */
      }
      break;
    case LVAL_LOCAL_REF:
      return BinaryenLocalGet(wasm->module, lval->offset, make_type_int32(1));
    default:
      return compile_lval_literal(wasm, lval);
  }
  return NULL;
}

void add_wasm_function(Wasm* wasm, char* fn_name, int params_count,
                       int locals_count, Ber body, int results_count) {
  BinaryenModuleRef module = wasm->module;

  BinaryenType params_type = make_type_int32(params_count);
  BinaryenType results_type = make_type_int32(results_count);
  /* BinaryenType results_type = make_results_type(results_count); */

  BinaryenType* local_types = make_type_int32_array(locals_count);

  BinaryenAddFunction(module, fn_name, params_type, results_type, local_types,
                      locals_count, body);
}

int compile_function(Wasm* wasm, Lval* lval_sym, Lval* lval) {
  /* BinaryenModuleRef module = wasm->module; */

  printf("fn-name: %s\n", lval_sym->str);
  printf("params: ");
  lval_println(lval->params);
  printf("body: ");
  lval_println(lval->body);

  char* fn_name = lval_sym->str;

  // Params

  int params_count = list_count(lval->params->head);

  Lenv* params_env = enter_env(wasm);
  Cell* cell = lval->params->head;
  int i = 0;
  while (cell) {
    Lval* lval_sym = cell->car;
    printf("ref count of lval_sym %d\n", get_ref_count(lval_sym));
    lenv_put(params_env, retain(lval_sym), make_lval_local_ref(PARAM, i++));
    cell = cell->cdr;
  }

  CONTEXT_FUNCTION("compile_function", lval_sym->str, params_count)

  // Compile
  Ber arg_list_head = compile_do_list(wasm, lval->body, RETURN_ON_ERROR);

  // Post compile
  leave_env(wasm);

  /* printf("local_count: %d\n", *context->local_count); */
  int locals_count = *context->local_count - params_count;

  Ber body = arg_list_head;
  lval->offset = add_fn_to_table(wasm, fn_name);

  int results_count = 1;
  add_wasm_function(wasm, fn_name, params_count, locals_count, body,
                    results_count);
  return lval->offset;
}

void print_pair(Lval* lval_sym, Lval* lval) {
  lval_print(lval_sym);
  printf(": ");
  lval_print(lval);
}

void process_env(Wasm* wasm) {
  Cell* cell = wasm->env->kv;
  if (!cell || !cell->car) return;
  printf("Processing env ==============================\n");
  while (cell) {
    Cell* pair = cell->car;
    Lval* lval_sym = (Lval*)((Cell*)pair)->car;
    Lval* lval = (Lval*)((Cell*)pair)->cdr;

    /* print_pair(lval_sym, lval); */
    if (lval->type == LVAL_FUNCTION) {
      compile_function(wasm, lval_sym, lval);
    }

    cell = cell->cdr;
    if (cell) printf("%s", "\n");
  }
  printf("\n");

  // walk list and encode every non function into a wasm data string, and keep
  // tabs on the offset for each symbol.

  // For functions: create a wasm function for each. Any symbol encountered
  // resolve to lval in wasm data by offset, unless redefined in fn already.
  // Any functions encountered again create a wasm function, but now with
  // closure vars added.
}

int compile(char* file_name) {
  info("Compiling %s\n", file_name);
  /* print_slot_size(); */

  set_log_level(LOG_LEVEL_INFO);

  Wasm* wasm = init_wasm();
  /* printf("__data_end: %d\n", wasm->__data_end); */
  wasm->env = interprete_file(file_name);

  import_runtime(wasm);

  process_env(wasm);

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
