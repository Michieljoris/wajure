#ifndef __BINARYEN_KITCHEN_SINK_H_
#define __BINARYEN_KITCHEN_SINK_H_

#include <binaryen-c.h>

BinaryenExpressionRef makeInt32(BinaryenModuleRef module, int x);

void test_types();

void test_features();
void test_core();
void test_unreachable();
void test_relooper();
void test_binaries();
void test_interpret();
void test_nonvalid();
void test_color_status();
void test_for_each();
#endif  // __BINARYEN_KITCHEN_SINK_H_
