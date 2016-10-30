// RUN: %dis %gta3sc %s --config=gta3 -fsyntax-only 2>&1 | %verify %s

VAR_INT x

// WHILE x = 0

WHILE x = 0    // expected-error {{ENDWHILE}}
/*
    WHILE x = 0
    /*
        WHILE x = 0
        /*
            WHILE x = 0
        */
        WHILE x = 0
    */
    WHILE x = 0
*/

/* 0 */ x = /* ?? */ 0 /* + ?? */
x = 0 // + ??

TERMINATE_THIS_SCRIPT