// # Check IR2 with and without OATC
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - -fcleo | %FileCheck %s
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - --cs -D CS | %FileCheck %s
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - --cm -D CM | %FileCheck %s
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - -fcleo -moatc | %FileCheck %s
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - --cs -D CS -moatc | %FileCheck %s
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - --cm -D CM -moatc | %FileCheck %s
//
// # Check the binary representation
// RUN: mkdir "%/T/cheader_oatc" || echo _
// RUN: %gta3sc %s --config=gtasa --guesser -o "%/T/cheader_oatc/c1.cs" --cs -D CS -moatc
// RUN: %checksum "%/T/cheader_oatc/c1.cs" 7e42cec2b9d51489e8c87ad4987759ed
//
#ifdef CS
SCRIPT_START
#endif
#ifdef CM
MISSION_START
#endif
{
LVAR_INT x

// Skip SCM Header
// CHECK: WAIT 100i8
WAIT 100

// CHECK-NEXT-L: WRITE_MEMORY 0i8 1i8 0i8 0i8
WRITE_MEMORY 0 1 0 FALSE

// CHECK-NEXT-L: MAIN_1:
loop1:
// CHECK-NEXT-L: WAIT 0i8
WAIT 0
// CHECK-NEXT-L: IS_GAME_VERSION_ORIGINAL
IS_GAME_VERSION_ORIGINAL
// CHECK-NEXT-L: IS_GAME_VERSION_ORIGINAL
IS_GAME_VERSION_ORIGINAL // using the same command twice shouldn't put it in the header twice
// CHECK-NEXT: GOTO_IF_FALSE [@%]MAIN_1
GOTO_IF_FALSE loop1

// CHECK-NEXT-L: MAIN_2:
loop2:
// CHECK-NEXT: GET_LABEL_POINTER [@%]MAIN_1 \d+@
GET_LABEL_POINTER loop1 x
// CHECK-NEXT: GOTO [@%]MAIN_2
GOTO loop2
}
#ifdef CM
MISSION_END
#endif
#ifdef CS
SCRIPT_END
#endif