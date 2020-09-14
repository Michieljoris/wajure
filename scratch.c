#include "scratch.h"

// ==================================================
void foo(BinaryenModuleRef module) {
  // Create a function type for  i32 (i32, i32)
  BinaryenType ii[3] = {BinaryenTypeInt32(), BinaryenTypeInt32(),
                        BinaryenTypeInt32()};
  BinaryenType params = BinaryenTypeCreate(ii, 3);
  BinaryenType results = BinaryenTypeInt32();

  // Get the 0 and 1 arguments, and add them
  BinaryenExpressionRef x = BinaryenLocalGet(module, 0, BinaryenTypeInt32()),
                        y = BinaryenLocalGet(module, 1, BinaryenTypeInt32());
  BinaryenExpressionRef add = BinaryenBinary(module, BinaryenAddInt32(), x, y);
  BinaryenExpressionRef add1 = BinaryenBinary(module, BinaryenAddInt32(), x, y);
  BinaryenType localTypes[] = {BinaryenTypeInt32(), BinaryenTypeInt32()};

  const char* switchValueNames[] = {"v1", "v2"};
  const char* switchBodyNames[] = {"the-nothing"};

  BinaryenExpressionRef
      /* temp1 = make_int32(module, 1), */
      /*                   temp2 = make_int32(module, 2), */
      /*                   temp3 = make_int32(module, 3), */
      /*                   temp4 = make_int32(module, 4), */
      /*                   temp5 = make_int32(module, 5), */
      /*                   temp6 = make_int32(module, 0), */
      /*                   temp7 = make_int32(module, 1), */
      temp8 = make_int32(module, 0),
      temp9 = make_int32(module, 1), temp10 = make_int32(module, 1);
  /* temp11 = make_int32(module, 3), */
  /* temp12 = make_int32(module, 5), */
  /* temp13 = make_int32(module, 10), */
  /* temp14 = make_int32(module, 11), */
  /* temp15 = make_int32(module, 110); */

  BinaryenExpressionRef s1 =
      BinaryenSwitch(module, switchValueNames, 2, "the-value", temp8, temp9);
  BinaryenExpressionRef s2 = BinaryenSwitch(
      module, switchBodyNames, 1, "the-nothing", make_int32(module, 2), NULL);

  BinaryenExpressionRef select1 =
      BinaryenSelect(module, temp8, temp9, temp10, BinaryenTypeAuto());

  BinaryenExpressionRef my_value_list[] = {
      /* BinaryenLocalSet(module, 5, BinaryenPop(module,
BinaryenTypeExnref())),
       */
      /* add, */
      /* add1, */
      /* make_memory_init(module), */
      s1,  // s2
      select1
      /* makeMemoryInit(module), */
      /* makeMemoryCopy(module), */
      /* makeMemoryFill(module), */
  };
  BinaryenExpressionRef block =
      BinaryenBlock(module, "my-block", my_value_list,
                    sizeof(my_value_list) / sizeof(BinaryenExpressionRef),
                    BinaryenTypeAuto());

  /* BinaryenExpressionPrint(my_value_list[0]); */

  /* BINARYEN_API void BinaryenSetStart(BinaryenModuleRef module, */
  /*                                    BinaryenFunctionRef start); */

  BinaryenAddFunction(module, "adder", params, results, localTypes,
                      sizeof(localTypes) / sizeof(BinaryenType), block);
}

BinaryenSetDebugInfo(1);
BINARYEN_API void BinaryenExpressionPrint(BinaryenExpressionRef expr);

BINARYEN_API BinaryenExpressionRef BinaryenDrop(BinaryenModuleRef module,
                                                BinaryenExpressionRef value);
Return
    : value can be NULL

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
      /*                           BinaryenTypeInt32(), 0); */ /*   BinaryenAddGlobal(module,"a-global",
                                     BinaryenTypeInt32(),
                                     0, */
      /*                     make_int32(module, 7)); */

      /*   BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(),
       * 1,
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
      /*       module, "lalloc_size", operands_lalloc, 1, make_type_int32(1));
       */

      /*   // TODO: see if str already exists and return that offset instead of
       * adding
       */
      /*   // str again to data */
      /*   int offset = add_string_to_data(wasm, lval->str); */

      /*   BER operands_strcpy[2] = {lalloc_size, */
      /*                                               wasm_offset(wasm,
       * offset)};
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
      /*       /\*     module, "_strcpy", operands_strcpy, 2,
       * make_type_int32(1));
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
      /*       module, func_name, operands, operands_count, make_type_int32(1));
       */

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
      /*     module, "new_lval_list", lval_list_operands, 1,
       * make_type_int32(1));
       */

      /* BER lval_println_operands[1] = {lval_list}; */
      /* BER lval_println_operands[1] = {wasm_offset(wasm, 22)};
       */
      /* BER body = BinaryenCall( */
      /*     module, "lval_println", lval_println_operands, 1,
       * BinaryenTypeNone());
       */

      /* BER print_slot_size = */
      /*     BinaryenCall(module, "print_slot_size", NULL, 0,
       * BinaryenTypeNone());
       */
      /* body = print_slot_size; */

      /* printf("fn-name: %s\n", lval_sym->str); */
      /* printf("params: "); */
      /* lval_println(lval->params); */
      /* printf("body: "); */
      /* lval_println(lval->body); */

      // Load: align can be 0, in which case it will be the natural alignment
      // (equal to bytes)
      /* BINARYEN_API BinaryenExpressionRef BinaryenLoad(BinaryenModuleRef
       * module,
       */
      /*                                                 uint32_t bytes, */
      /*                                                 int8_t signed_, */
      /*                                                 uint32_t offset, */
      /*                                                 uint32_t align, */
      /*                                                 BinaryenType type, */
      /*                                                 BinaryenExpressionRef
       * ptr);
       */
      /* // Store: align can be 0, in which case it will be the natural
       * alignment (equal */
      /* // to bytes) */
      /* BINARYEN_API BinaryenExpressionRef BinaryenStore(BinaryenModuleRef
       * module,
       */
      /*                                                  uint32_t bytes, */
      /*                                                  uint32_t offset, */
      /*                                                  uint32_t align, */
      /*                                                  BinaryenExpressionRef
       * ptr,
       */
      /*                                                  BinaryenExpressionRef
       * value,
       */
      /*                                                  BinaryenType type); */

      /* void add_wasm_function(Wasm* wasm, char* fn_name, int params_count, */
      /*                        int locals_count, Ber body, int results_count) {
       */
      /*   BinaryenModuleRef module = wasm->module; */

      /*   BinaryenType params_type = make_type_int32(params_count); */
      /*   BinaryenType results_type = make_type_int32(results_count); */
      /*   /\* BinaryenType results_type = make_results_type(results_count); *\/
       */

      /*   BinaryenType* local_types = make_type_int32_array(locals_count); */

      /*   BinaryenAddFunction(module, fn_name, params_type, results_type,
       * local_types, */
      /*                       locals_count, body); */
      /* } */

      /* TODO: */
      //
      // call root fn
      // check whether wasm ref is a a lambda/set/map/vector, print error msg if
      // not datafy sys and root fns

      /* Ber wval_operands[] = {lval_wasm_ref}; */
      /* Ber wval_print = BinaryenCall(module, "wval_print", wval_operands, 1,
       */
      /*                               BinaryenTypeNone()); */
      /* wasm_args[i++] = wval_print; */

      /* BinaryenOp ge = BinaryenGeUInt32(); */
      /* Ber ifge = BinaryenBinary(module, ge, args_count, wasm_param_count); */
      /* Ber if = BinaryenIf(module, condition, if_true, if_false); */

      /* BinaryenModuleRef module = wasm->module; */

      /* printf("fn-name: %s\n", lval_sym->str); */
      /* printf("params: "); */
      /* lval_println(lval_fun->params); */
      /* printf("body: "); */
      /* lval_println(lval_fun->body); */

      // Write wasm that does the following:
      // Compile the args to wasm, put them on lispy stack in reverse order,
      // with count of args on top slot.

      // Check whether the local ref is a lambda (which might be a wrapper for
      // sys and named fns)
      //
      // Write fns to deal with keyword, vector, set and map, dispatch by type
      // and pass first node plus args to fn

      // It's the callee's responsibility to check correct number of args (so
      // could be disabled by flag). Without flag it's possible to call a fn
      // with too few args, which would have unpredictable results. Too many
      // doesn't hurt necessarily. It's also callee's responsibility to wrap up
      // excess args into a rest arg. Otherwise we would have to check for this
      // for every fn call. Throw error if count of args < param count (without
      // any rest arg): [a b] has param count of 2, but [a b & c] has param
      // count of 2 as well. If fn has rest arg then insert code that gathers
      // excess args up into list and stores that list in a local var to be
      // referred to in the rest of the fn. Only insert param checking code when
      // lispy compiler flag is set for it. Same for rest arg, only insert wasm
      // code for it when appropriate.

      // When a LVAL_WASM_LAMBDA is a result of a partial its fn_table_index
      // points to a wrapper fn that puts the partial args after the current
      // stack pointer and increases sp by partial_count, and then calls the
      // actual fn (which again might be a partial).

      // Call indirect fn with the right offset, and one arg (pointer to
      // closure) Set stack pointer to free mem; (so add param_count + 1  to sp,
      // last slot holds param_count itself). On return from this call set stack
      // pointer back.

      // Resolve symbol in our compiler env and compile it. At runtime there's
      // no notion of environments or symbols that resolve to other lvalues.
      // Symbols at runtime are just that, a literal similar to strings, numbers
      // etc
      /* CResult resolve_symbol_and_compile(Wasm* wasm, Lval* lval_sym) { */
      /*   /\* printf(">>>>>>>>>>>>\n"); *\/ */
      /*   /\* printf("COMPILING SYMBOL!!! %s\n", lval_sym->str); *\/ */
      /*   Lval* lval_resolved_sym = lenv_get(wasm->env, lval_sym); */
      /*   /\* lval_println(lval_resolved_sym); *\/ */
      /*   switch (lval_resolved_sym->type) { */
      /*     case LVAL_ERR: */
      /*       return quit(wasm, lval_resolved_sym->str); */
      /*     case LVAL_COMPILER:  // symbol has been added to env while
       * compiling, so
       */
      /*       // it's a closure var, param or local (let) var */
      /*       return compile_lval_compiler(wasm, lval_sym, lval_resolved_sym);
       */
      /*     case LVAL_FUNCTION:  // as evalled in our compiler env */
      /*       switch (lval_resolved_sym->subtype) { */
      /*         case SYS: */
      /*           return datafy_sys_fn(wasm, lval_resolved_sym); */
      /*         case LAMBDA:  // functions in compiler env */
      /*           return compile_global_lambda(wasm, lval_sym->str,
       * lval_resolved_sym); */
      /*         case SPECIAL: */
      /*           return quit(wasm, */
      /*                       "ERROR: Can't take value of a special form such
       * as %s",
       */
      /*                       lval_resolved_sym->str); */
      /*         case MACRO: */
      /*           return quit(wasm, "ERROR: Can't take value of a macro such as
       * %s",
       */
      /*                       lval_resolved_sym->str); */
      /*         default:; */
      /*           return quit(wasm, "ERROR: Can't compile function with subtype
       * %d\n", */
      /*                       lval_resolved_sym->subtype); */
      /*       } */
      /*       break; */
      /*     default:  // as evalled in our compiler env */
      /*       return datafy_lval(wasm, lval_resolved_sym); */
      /*       /\* case LVAL_COLLECTION: *\/ */
      /*       /\*   return datafy_collection(wasm, lval_resolved_sym); *\/ */
      /*       /\* case LVAL_SYMBOL: *\/ */
      /*       /\* case LVAL_LITERAL: *\/ */
      /*       /\*   return datafy_literal(wasm, lval_resolved_sym); *\/ */
      /*   } */

      /*   /\* printf("resolved sym: \n"); *\/ */
      /*   /\* lval_println(lval_resolved_sym); *\/ */
      /* } */
