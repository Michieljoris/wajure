/* /\* #include "binaryen.h" *\/ */

/* /\* #include "binaryen_hello_world.h" *\/ */

#include <binaryen-c.h>

// "hello world" type example: create a function that adds two i32s and
/* returns */
// the result

int make_bmodule() {
  BinaryenModuleRef module = BinaryenModuleCreate();

  // Create a function type for  i32 (i32, i32)
  BinaryenType ii[3] = {BinaryenTypeInt32(), BinaryenTypeInt32(),
                        BinaryenTypeInt32()};
  BinaryenType params = BinaryenTypeCreate(ii, 3);
  BinaryenType results = BinaryenTypeInt32();

  // Get the 0 and 1 arguments, and add them
  BinaryenExpressionRef x = BinaryenLocalGet(module, 0, BinaryenTypeInt32()),
                        y = BinaryenLocalGet(module, 1, BinaryenTypeInt32());
  BinaryenExpressionRef add = BinaryenBinary(module, BinaryenAddInt32(), x, y);
  BinaryenType localTypes[] = {BinaryenTypeInt32(),   BinaryenTypeInt32(),
                               BinaryenTypeInt64(),   BinaryenTypeInt32(),
                               BinaryenTypeFloat32(), BinaryenTypeFloat64(),
                               BinaryenTypeInt32()};

  // Create the add function
  // Note: no additional local variables
  // Note: no basic blocks here, we are an AST. The function body is just an
  // expression node.
  BinaryenAddFunction(module, "adder", params, results, localTypes,
                      sizeof(localTypes) / sizeof(BinaryenType), add);

  // Print it out
  BinaryenModulePrint(module);

  // Clean up the module, which owns all the objects we created above
  BinaryenModuleDispose(module);

  return 0;
}

/* (module */
/*  (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32))) */
/*  (func $adder (param $0 i32) (param $1 i32) (param $2 i32) (result i32) */
/*   (i32.add */
/*    (local.get $0) */
/*    (local.get $1) */
/*   ) */
/*  ) */
/* ) */
