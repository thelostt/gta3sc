// RUN: %gta3sc %s --config=gtavc -moptimize-zero -emit-ir2 | %FileCheck %s

// CHECK-L: SET_TIME_SCALE 0i8
SET_TIME_SCALE 0.0

TERMINATE_THIS_SCRIPT