// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - | %FileCheck %s

{
    // CHECK-L: SET_LVAR_INT 32@ 1i8
    timera = 1
    // CHECK-L: SET_LVAR_INT 33@ 2i8
    timerb = 2
}

TERMINATE_THIS_SCRIPT