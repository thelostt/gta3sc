// RUN: %gta3sc %s --config=gta3 -emit-ir2 -o - | %FileCheck %s
// RUN: %gta3sc %s --config=gtavc -emit-ir2 -o - | %FileCheck %s
{
VAR_INT gi
LVAR_INT li
VAR_FLOAT gf
LVAR_FLOAT lf

// CHECK-L: ANDOR 21i8
// CHECK-NEXT: IS_INT_VAR_EQUAL_TO_INT_VAR 0@ &8
// CHECK-NEXT: IS_FLOAT_VAR_EQUAL_TO_FLOAT_VAR 1@ &12
IF li = gi
OR lf = gf
    NOP
ENDIF

TERMINATE_THIS_SCRIPT
}