// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - | %FileCheck %s

VAR_TEXT_LABEL label
VAR_TEXT_LABEL16 text16

// CHECK-L: PRINT_HELP 'LABEL'
PRINT_HELP label
// CHECK-L: PRINT_HELP s&8
PRINT_HELP $label

// CHECK-L: STRING_CAT16 "LABEL" s&8 v&16
STRING_CAT16 label $label text16

TERMINATE_THIS_SCRIPT