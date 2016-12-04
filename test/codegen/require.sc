// TODO -frequire
// RUN: %gta3sc %s --config=gta3 -emit-ir2 -o - | %FileCheck %s
REQUIRE req1.sc
GOSUB req1_label2
TERMINATE_THIS_SCRIPT