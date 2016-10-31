// RUN: %gta3sc %s --config=gtavc -emit-ir2 | %FileCheck %s

// TODO test all the expression alternators (maybe in alternators.sc)

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

// Test VAR = CONST
{
    // CHECK: IS_THING_GREATER_THAN_THING &16 [\di]+
    // CHECK: IS_THING_EQUAL_TO_THING &16 [\di]+
    IF k > FBI
    OR k = ARMY
        // CHECK: SET &16 [\di]+
        k = SWAT
    ENDIF
}

// Test Increment/Decrement
{
    // CHECK-L:      ADD_THING_TO_THING &8 1i8
    ++i
    // CHECK-NEXT-L: SUB_THING_FROM_THING &8 1i8
    --i
}

TERMINATE_THIS_SCRIPT