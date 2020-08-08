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
Return : value can be NULL
