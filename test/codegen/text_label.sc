// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 | %FileCheck %s

VAR_TEXT_LABEL label

// CHECK-L: PRINT_HELP 'LABEL'
PRINT_HELP label
// CHECK-L: PRINT_HELP s&8
PRINT_HELP $label

TERMINATE_THIS_SCRIPT