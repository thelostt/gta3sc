// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 | %FileCheck %s

{
    // CHECK-L: SET 32@ 1i8
    timera = 1
    // CHECK-L: SET 33@ 2i8
    timerb = 2
}

TERMINATE_THIS_SCRIPT