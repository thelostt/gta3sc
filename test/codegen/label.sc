// RUN: %gta3sc %s --config=gta3 -emit-ir2 | %FileCheck %s

VAR_INT x

IF x = 0
    // CHECK-L:      WAIT 1i8
    // CHECK-NEXT-L: GOTO @MAIN_2
    WAIT 1
    GOTO label2
ENDIF

// CHECK-L:      MAIN_1:
// CHECK-NEXT-L: WAIT 100i8
// CHECK-NEXT-L: GOTO @MAIN_1
label1:
WAIT 100
GOTO label1

// CHECK-L:      MAIN_2:
// CHECK-NEXT-L: WAIT 200i16
// CHECK-NEXT-L: GOTO @MAIN_1
label2:
WAIT 200
GOTO label1
