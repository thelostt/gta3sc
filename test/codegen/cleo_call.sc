// RUN: %gta3sc %s --config=gtasa --guesser --cs -emit-ir2 -o - | %FileCheck %s
{
LVAR_INT a
LVAR_TEXT_LABEL text

// CHECK-L: CLEO_CALL %MAIN_1 2i8 5i8 5i8 0@
CLEO_CALL sum 0 5 5 a

// CHECK-L: CLEO_CALL %MAIN_1 2i8 "TEXT" 1@s 0@
CLEO_CALL sum 0 text $text a	// sums two pointers

// CHECK-L: CLEO_CALL %MAIN_1 2i8 "Test Case" 1@s 0@
CLEO_CALL sum 0 "Test Case" $text a	// sums two pointers

TERMINATE_THIS_CUSTOM_SCRIPT
}

{
	sum:
	LVAR_INT a b c
	c = a + b
	// CHECK-L: CLEO_RETURN 1i8 2@
	CLEO_RETURN 0 c
}
