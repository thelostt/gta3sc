// RUN: %gta3sc %s --config=gta3 -fswitch --guesser -emit-ir2 | %FileCheck %s
// RUN: %gta3sc %s --config=gtavc -fswitch --guesser -emit-ir2 | %FileCheck %s
VAR_INT n

// Using a default case, and an out of order case.
{
    SWITCH n
        // CHECK-L:      IS_THING_EQUAL_TO_THING &8 100i8
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_1
        CASE 100
            // CHECK-NEXT-L: WAIT 100i8
            WAIT 100
            // CHECK-NEXT-L: GOTO @MAIN_5
            BREAK
        // CHECK-NEXT-L: MAIN_1:
        // CHECK-NEXT-L: IS_THING_EQUAL_TO_THING &8 200i16
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_2
        CASE 200
            // CHECK-NEXT-L: WAIT 200i16
            WAIT 200
            // CHECK-NEXT-L: GOTO @MAIN_5
            BREAK
        // CHECK-NEXT-L: MAIN_2:
        // CHECK-NEXT-L: IS_THING_EQUAL_TO_THING &8 300i16
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_3
        CASE 300
            // CHECK-NEXT-L: WAIT 300i16
            WAIT 300
            // CHECK-NEXT-L: GOTO @MAIN_5
            BREAK
        // CHECK-NEXT-L: MAIN_3:
        // CHECK-NEXT-L: IS_THING_EQUAL_TO_THING &8 50i8
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_4
        CASE 50
            // CHECK-NEXT-L: WAIT 50i8
            WAIT 50
            // CHECK-NEXT-L: GOTO @MAIN_5
            BREAK
        // CHECK-NEXT-L: MAIN_4:
        DEFAULT
            // CHECK-NEXT-L: WAIT 0i8
            WAIT 0
            // CHECK-NEXT-L: GOTO @MAIN_5
            BREAK
    ENDSWITCH
}


// Using no default case, and an out of order case.
{
    // CHECK-NEXT-L: MAIN_5:
    SWITCH n
        // CHECK-NEXT-L: IS_THING_EQUAL_TO_THING &8 100i8
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_6
        CASE 100
            // CHECK-NEXT-L: WAIT 100i8
            WAIT 100
            // CHECK-NEXT-L: GOTO @MAIN_8
            BREAK
        // CHECK-NEXT-L: MAIN_6:
        // CHECK-NEXT-L: IS_THING_EQUAL_TO_THING &8 200i16
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_7
        CASE 200
            // CHECK-NEXT-L: WAIT 200i16
            WAIT 200
            // CHECK-NEXT-L: GOTO @MAIN_8
            BREAK
        // CHECK-NEXT-L: MAIN_7:
        // CHECK-NEXT-L: IS_THING_EQUAL_TO_THING &8 50i8
        // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_8
        CASE 50
            // CHECK-NEXT-L: WAIT 50i8
            WAIT 50
            // CHECK-NEXT-L: GOTO @MAIN_8
            BREAK
    ENDSWITCH
}

// CHECK-NEXT-L: MAIN_8:
// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT
TERMINATE_THIS_SCRIPT