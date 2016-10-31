// RUN: %gta3sc %s --config=gta3 -mskip-if -emit-ir2 | %FileCheck %s

VAR_INT x

// CHECK-NOT-L: ANDOR 0i8
// CHECK-L:     IS_THING_EQUAL_TO_THING &8 0i8
IF x = 0
    WAIT 0
ENDIF

// CHECK-NOT-L: ANDOR 0i8
// CHECK-L:     IS_THING_EQUAL_TO_THING &8 0i8
WHILE x = 0
    WAIT 0
ENDWHILE

TERMINATE_THIS_SCRIPT

