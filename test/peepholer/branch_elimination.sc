// RUN: %gta3sc %s -O --config=gta3 -emit-ir2 -o - | %FileCheck %s

VAR_INT x

//=== Replace {GOTO_IF_FALSE -> GOTO -> BLOCK} with {GOTO_IF_FALSE -> BLOCK}. ===//
// CHECK-NEXT-L: MAIN_1:
// CHECK-NEXT-L: ANDOR 0i8
// CHECK-NEXT-L: IS_INT_VAR_GREATER_THAN_NUMBER &8 0i8
// CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_2
WHILE x > 0
    // CHECK-NEXT-L: ANDOR 0i8
    // CHECK-NEXT-L: IS_INT_VAR_EQUAL_TO_NUMBER &8 1i8
    // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_1
    //                            ^~~~~~~~~ Optimized Branch
    IF x = 1
        // CHECK-NEXT-L: WAIT 0i8
        WAIT 0
    ENDIF
    // CHECK-NEXT-L: GOTO @MAIN_1
ENDWHILE

//=== Replace {GOTO -> GOTO -> BLOCK} with {GOTO -> BLOCK}. ===//
// CHECK-NEXT-L: MAIN_2:
// CHECK-NEXT-L: ANDOR 0i8
// CHECK-NEXT-L: IS_INT_VAR_GREATER_THAN_NUMBER &8 0i8
// CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_4
WHILE x > 0
    // CHECK-NEXT-L: ANDOR 0i8
    // CHECK-NEXT-L: IS_INT_VAR_EQUAL_TO_NUMBER &8 1i8
    // CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_3
    IF x = 1
        // CHECK-NEXT-L: WAIT 11i8
        WAIT 11
        // CHECK-NEXT-L: GOTO @MAIN_2
        //                    ^~~~~~~~ Optimized Branch
    ELSE
        // CHECK-NEXT-L: MAIN_3:
        // CHECK-NEXT-L: WAIT 12
        WAIT 12
    ENDIF
    // CHECK-NEXT-L: GOTO @MAIN_2
ENDWHILE

//=== Test GOTO indirections (within a certain limit) ===//
IF x > 3
    IF x > 4
        IF x > 5
            IF x > 6
                IF x > 7
                    IF x > 8
                        // CHECK-L: WAIT 81i8
                        WAIT 81
                        // CHECK-L: GOTO @MAIN_10
                        //               ^~~~~~~~ Optimized Branch Over Limit
                    ELSE
                        WAIT 80
                        // CHECK-L: GOTO @MAIN_12
                        //               ^~~~~~~~ Optimized Branch
                    ENDIF
                ELSE
                    WAIT 70
                    // CHECK-L: GOTO @MAIN_12
                    //               ^~~~~~~~ Optimized Branch
                ENDIF
            ELSE
                WAIT 60
                // CHECK-L: GOTO @MAIN_12
                //               ^~~~~~~~ Optimized Branch
            ENDIF
        ELSE
            WAIT 50
            // CHECK-L: GOTO @MAIN_12
            //               ^~~~~~~~ Optimized Branch
        ENDIF
    ELSE
        WAIT 40
        // CHECK-L: MAIN_10:
        // CHECK-L: GOTO @MAIN_12
        //               ^~~~~~~~ Optimized Branch
    ENDIF
ELSE
    // CHECK-L: WAIT 30
    WAIT 30
ENDIF

// CHECK-NEXT-L: MAIN_12:
// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT
TERMINATE_THIS_SCRIPT