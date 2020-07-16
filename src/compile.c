#include "compile.h"

#include <binaryen-c.h>

#include "io.h"
#include "iter.h"
#include "lispy_mempool.h"
#include "misc_fns.h"
#include "print.h"
#include "read.h"
#include "refcount.h"

BinaryenExpressionRef make_int32(BinaryenModuleRef module, int x) {
  return BinaryenConst(module, BinaryenLiteralInt32(x));
}

BinaryenExpressionRef make_memory_init(BinaryenModuleRef module) {
  BinaryenExpressionRef dest = make_int32(module, 1836);
  BinaryenExpressionRef offset = make_int32(module, 0);
  BinaryenExpressionRef size = make_int32(module, 12);
  return BinaryenMemoryInit(module, 1, dest, offset, size);
};

char* make_err(char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  char* str = lalloc_size(512);
  vsnprintf(str, 511, fmt, va);
  str = lrealloc(str, _strlen(str) + 1);
  va_end(va);
  return str;
}

// import: memory, table, data_end, make_lval

int compile_expression(BinaryenModuleRef module, Lval* lval_expr) {
  lval_println(lval_expr);

  /* info("\n->>>>>>>>EVAL: "); */
  /* lval_infoln(lval); */
  /* printf("lval->type: %s\n", lval_type_constant_to_name(lval->type)); */
  /* Lval* ret = NIL; */
  /* switch (lval->type) { */
  /*   case LVAL_SYMBOL: */
  /*     return eval_symbol(env, lval); */
  /*   case LVAL_COLLECTION: */
  /*     switch (lval->subtype) { */
  /*       case LIST: */
  /*         return compile_fn_call(env, lval); */
  /*       case VECTOR: */
  /*         return compile_vector(env, lval); */
  /*       case MAP: */
  /*         /\* TODO: *\/ */
  /*         return lval; */
  /*       default: */
  /*         return make_err("Unknown seq subtype: %d ", lval->subtype); */
  /*     } */
  /*     break; */
  /*   default: */
  /*     return retain(lval); */
  /* } */
  return 1;
}

void add_memory_section(BinaryenModuleRef module) {
  BinaryenAddMemoryImport(module, "memory", "env", "memory", 0);
  const char* segments[] = {"hello, world\n" /* , "Hello world again!" */};
  int8_t segmentPassive[] = {0};
  BinaryenExpressionRef __data_end =
      BinaryenGlobalGet(module, "__data_end", BinaryenTypeInt32());

  BinaryenExpressionRef segmentOffsets[] = {/* make_int32(module, 1836) */
                                            __data_end
                                            /* __data_end *\/ */};

  BinaryenIndex segmentSizes[] = {13 /* , 18 */};
  int initial_mem_size = 2;
  int max_mem_size = 32767;
  int num_segments = 1;
  int shared = 0;
  BinaryenSetMemory(module, initial_mem_size, max_mem_size, "mem", segments,
                    segmentPassive, segmentOffsets, segmentSizes, num_segments,
                    shared);
}

void add_global_section(BinaryenModuleRef module) {
  BinaryenAddGlobalImport(module, "data_end", "env", "data_end",
                          BinaryenTypeInt32(), 0);

  BinaryenAddGlobal(module, "a-global", BinaryenTypeInt32(), 0,
                    make_int32(module, 7));

  BinaryenAddGlobal(module, "a-mutable-global", BinaryenTypeFloat32(), 1,
                    make_int32(module, 123));
}

void add_function_table(BinaryenModuleRef module) {
  BinaryenAddTableImport(module, "table", "env", "table");
  int initial = 1;
  int maximum = 1;

  const char* funcNames[] = {"some_func"};
  int numFuncNames = 1;
  BinaryenExpressionRef offset = make_int32(module, 11);
  BinaryenSetFunctionTable(module, initial, maximum, funcNames, numFuncNames,
                           offset);
}

void add_function(BinaryenModuleRef module) {
  BinaryenType _printf_params[2] = {BinaryenTypeInt32(), BinaryenTypeInt32()};
  BinaryenType printf_params = BinaryenTypeCreate(_printf_params, 2);
  BinaryenAddFunctionImport(module, "printf", "env", "printf", printf_params,
                            BinaryenTypeNone());
}

int compile_list(BinaryenModuleRef module, Lval* list) {
  scoped_iter Cell* i = iter_new(list);
  Lval* lval = iter_next(i);
  int result;
  while (lval) {
    result = compile_expression(module, lval);
    if (!result) {
      lval_print(lval);
      return result;
    }
    lval = iter_next(i);
  }
  return 1;
}

void add_test_fn(BinaryenModuleRef module) {
  /* BinaryenType ii[3] = {BinaryenTypeInt32(), BinaryenTypeInt32(), */
  /* BinaryenTypeInt32()}; */
  /* BinaryenType params = BinaryenTypeCreate(ii, 3); */
  /* BinaryenType results = BinaryenTypeInt32(); */

  BinaryenType _printf_params[2] = {BinaryenTypeInt32(), BinaryenTypeInt32()};
  BinaryenType printf_params = BinaryenTypeCreate(_printf_params, 2);
  BinaryenType _log_params[1] = {BinaryenTypeInt32()};
  BinaryenType log_params = BinaryenTypeCreate(_log_params, 1);

  BinaryenAddFunctionImport(module, "printf", "env", "printf", printf_params,
                            BinaryenTypeInt32());

  BinaryenAddFunctionImport(module, "log", "env", "log", log_params,
                            BinaryenTypeNone());

  BinaryenAddFunctionImport(module, "init_malloc", "env", "init_malloc",
                            BinaryenTypeNone(), BinaryenTypeNone());

  BinaryenType params = BinaryenTypeNone();
  BinaryenType results = BinaryenTypeInt32();
  BinaryenType localTypes[] = {BinaryenTypeInt32(), BinaryenTypeInt32()};

  BinaryenExpressionRef __data_end =
      BinaryenGlobalGet(module, "__data_end", BinaryenTypeInt32());

  BinaryenExpressionRef operands[] = {__data_end, make_int32(module, 0)};
  /* BinaryenExpressionRef operands[] = {make_int32(module, 1836), */
  /*                                     make_int32(module, 0)}; */

  BinaryenExpressionRef printf =
      BinaryenCall(module, "printf", operands, 2, BinaryenTypeNone());

  BinaryenExpressionRef drop = BinaryenDrop(module, printf);

  BinaryenExpressionRef log_operands[] = {make_int32(module, 42)};
  BinaryenExpressionRef log =
      BinaryenCall(module, "log", log_operands, 1, BinaryenTypeNone());

  BinaryenExpressionRef im_operands[] = {};
  BinaryenExpressionRef im =
      BinaryenCall(module, "init_malloc", im_operands, 0, BinaryenTypeNone());
  /* (drop */
  /*    (call 6 */
  /*      (i32.const 1032) */
  /*      (i32.const 0))) */
  BinaryenExpressionRef my_value_list[] = {/* drop, */
                                           /* make_memory_init(module), */
                                           drop, log};
  BinaryenExpressionRef block =
      BinaryenBlock(module, "my-block", my_value_list,
                    sizeof(my_value_list) / sizeof(BinaryenExpressionRef),
                    BinaryenTypeAuto());

  BinaryenAddFunction(module, "test", params, results, localTypes,
                      sizeof(localTypes) / sizeof(BinaryenType), block);
}

void write_string(char* file_name, char* str) {
  FILE* f = fopen(file_name, "wb");
  fputs(str, f);
  fclose(f);
}
void write_binary(char* file_name, void* bin, size_t size) {
  FILE* f = fopen(file_name, "wb");
  fwrite(bin, 1, size, f);
  fclose(f);
}

int compile(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    info("Compiling %s\n", argv[i]);
    char* str = read_file(argv[i]);
    if (!str) {
      printf("Could not load file %s", str);
      return 0;
    }
    int pos = 0;
    scoped Lval* lval_list = lval_read_list(str, &pos, '\0');
    if (lval_list->type == LVAL_ERR) {
      lval_println(lval_list);
      return 0;
    }

    BinaryenModuleRef module = BinaryenModuleCreate();

    compile_list(module, lval_list);

    BinaryenAddGlobalImport(module, "__data_end", "env", "__data_end",
                            BinaryenTypeInt32(), 0);
    add_test_fn(module);

    BinaryenAddFunctionExport(module, "test", "test");

    // We only have this data _after_ compiling
    /* add_global_section(module); */
    /* add_function_table(module); */
    /* add_function(module); */
    add_memory_section(module);

    printf("mem id %d\n", BinaryenExternalMemory());
    BinaryenModulePrint(module);

    BinaryenSetColorsEnabled(0);
    char* output = BinaryenModuleAllocateAndWriteText(module);
    /* printf("%s %d\n", output, _strlen(output)); */
    write_string("compiled/lispy.wat", output);
    free(output);

    BinaryenModuleAllocateAndWriteResult result =
        BinaryenModuleAllocateAndWrite(module, NULL);

    void* binary = result.binary;
    size_t binaryBytes = result.binaryBytes;
    char* sourceMap = result.sourceMap;
    printf("%s \n%zu \n%s\n", binary, binaryBytes, sourceMap);
    write_binary("compiled/lispy.wasm", binary, binaryBytes);
    free(binary);
    free(sourceMap);

    BinaryenModuleDispose(module);
  }
  return 1;
}

// ==================================================
/* void foo(BinaryenModuleRef module) { */
/*   // Create a function type for  i32 (i32, i32) */
/*   BinaryenType ii[3] = {BinaryenTypeInt32(), BinaryenTypeInt32(), */
/*                         BinaryenTypeInt32()}; */
/*   BinaryenType params = BinaryenTypeCreate(ii, 3); */
/*   BinaryenType results = BinaryenTypeInt32(); */

/*   // Get the 0 and 1 arguments, and add them */
/*   BinaryenExpressionRef x = BinaryenLocalGet(module, 0, BinaryenTypeInt32()),
 */
/*                         y = BinaryenLocalGet(module, 1, BinaryenTypeInt32());
 */
/*   BinaryenExpressionRef add = BinaryenBinary(module, BinaryenAddInt32(), x,
 * y); */
/*   BinaryenExpressionRef add1 = BinaryenBinary(module, BinaryenAddInt32(), x,
 * y); */
/*   BinaryenType localTypes[] = {BinaryenTypeInt32(), BinaryenTypeInt32()}; */

/*   const char* switchValueNames[] = {"v1", "v2"}; */
/*   const char* switchBodyNames[] = {"the-nothing"}; */

/*   BinaryenExpressionRef */
/*       /\* temp1 = make_int32(module, 1), *\/ */
/*       /\*                   temp2 = make_int32(module, 2), *\/ */
/*       /\*                   temp3 = make_int32(module, 3), *\/ */
/*       /\*                   temp4 = make_int32(module, 4), *\/ */
/*       /\*                   temp5 = make_int32(module, 5), *\/ */
/*       /\*                   temp6 = make_int32(module, 0), *\/ */
/*       /\*                   temp7 = make_int32(module, 1), *\/ */
/*       temp8 = make_int32(module, 0), */
/*       temp9 = make_int32(module, 1), temp10 = make_int32(module, 1); */
/*   /\* temp11 = make_int32(module, 3), *\/ */
/*   /\* temp12 = make_int32(module, 5), *\/ */
/*   /\* temp13 = make_int32(module, 10), *\/ */
/*   /\* temp14 = make_int32(module, 11), *\/ */
/*   /\* temp15 = make_int32(module, 110); *\/ */

/*   BinaryenExpressionRef s1 = */
/*       BinaryenSwitch(module, switchValueNames, 2, "the-value", temp8, temp9);
 */
/*   BinaryenExpressionRef s2 = BinaryenSwitch( */
/*       module, switchBodyNames, 1, "the-nothing", make_int32(module, 2),
 * NULL); */

/*   BinaryenExpressionRef select1 = */
/*       BinaryenSelect(module, temp8, temp9, temp10, BinaryenTypeAuto()); */

/*   BinaryenExpressionRef my_value_list[] = { */
/*       /\* BinaryenLocalSet(module, 5, BinaryenPop(module,
 * BinaryenTypeExnref())), */
/*        *\/ */
/*       /\* add, *\/ */
/*       /\* add1, *\/ */
/*       /\* make_memory_init(module), *\/ */
/*       s1,  // s2 */
/*       select1 */
/*       /\* makeMemoryInit(module), *\/ */
/*       /\* makeMemoryCopy(module), *\/ */
/*       /\* makeMemoryFill(module), *\/ */
/*   }; */
/*   BinaryenExpressionRef block = */
/*       BinaryenBlock(module, "my-block", my_value_list, */
/*                     sizeof(my_value_list) / sizeof(BinaryenExpressionRef), */
/*                     BinaryenTypeAuto()); */

/*   /\* BinaryenExpressionPrint(my_value_list[0]); *\/ */

/*   /\* BINARYEN_API void BinaryenSetStart(BinaryenModuleRef module, *\/ */
/*   /\*                                    BinaryenFunctionRef start); *\/ */

/*   BinaryenAddFunction(module, "adder", params, results, localTypes, */
/*                       sizeof(localTypes) / sizeof(BinaryenType), block); */
/* } */

/* BinaryenSetDebugInfo(1); */
/* BINARYEN_API void BinaryenExpressionPrint(BinaryenExpressionRef
   expr); */

/* BINARYEN_API BinaryenExpressionRef BinaryenDrop(BinaryenModuleRef
 * module,
 */
/*                                                 BinaryenExpressionRef
 * value); */
// Return: value can be NULL
