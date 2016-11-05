// RUN: %gta3sc %s --config=gta3 -D TEST_SYMBOL -emit-ir2 -o - | %FileCheck %s

// CHECK-L: WAIT 1i8
#ifdef TEST_SYMBOL
WAIT 1
#endif

// CHECK-NOT-L: WAIT 2i8
#ifndef TEST_SYMBOL
WAIT 2
#endif

// CHECK-NOT-L: WAIT 3i8
#ifdef UNDEFINED_SYMBOL
WAIT 3
#endif

// CHECK-L: WAIT 4i8
#ifndef UNDEFINED_SYMBOL
WAIT 4
#endif

// CHECK-NOT-L: WAIT 5i8
#ifndef TEST_SYMBOL
#   ifdef TEST_SYMBOL
        WAIT 5
#   endif
#endif

// CHECK-L: TERMINATE_THIS_SCRIPT
TERMINATE_THIS_SCRIPT