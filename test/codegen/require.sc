// TODO -frequire
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - | %FileCheck %s

REQUIRE req1_from_main.sc
REQUIRE req1_from_main.sc			// Requiring multiple times has no effect
REQUIRE req1_from_main.sc			// Ditto.
REQUIRE req6_from_everywhere.sc

VAR_INT main_var
CREATE_OBJECT main_obj .0 .0 .0 main_var

LAUNCH_MISSION subscript.sc
LOAD_AND_LAUNCH_MISSION miss1.sc
REGISTER_STREAMED_SCRIPT stream1.sc

TERMINATE_THIS_SCRIPT