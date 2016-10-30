// Tests the codegeneration of expressions.
// RUN: %gta3sc %s --config=gta3 -emit-ir2 | %FileCheck %s

VAR_INT   i j k
VAR_FLOAT x y z

// Test commutation property
{
    // CHECK-L:      MULT_THING_BY_THING &8 2i8
    i = 2 * i
    // CHECK-NEXT-L: MULT_THING_BY_THING &8 2i8
    i = i * 2
    // CHECK-NEXT-L: ADD_THING_TO_THING &8 2i8
    i = 2 + i
    // CHECK-NEXT-L: ADD_THING_TO_THING &8 2i8
    i = i + 2
}

TERMINATE_THIS_SCRIPT