// This is a test about custom headers in multifiles. The sample header used is OATC.
//
// # Check IR2 with and without OATC
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - -fcleo | %FileCheck %s
// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 -o - -fcleo -moatc | %FileCheck %s
//
// # Check the binary representation
// RUN: mkdir "%/T/cheader_multifile" || echo _
// RUN: %gta3sc %s --config=gtasa --guesser -o "%/T/cheader_multifile/main.scm" -fcleo -moatc
// RUN: %checksum "%/T/cheader_multifile/main.scm" 0a46a795e750d46c61d358a9546a0454
// RUN: %checksum "%/T/cheader_multifile/script.img" 6c406015e6f70f9dc11571136a80b71a
//
VAR_INT v2

// CHECK-NEXT-L: #DEFINE_STREAM STREAM1 0
// CHECK-NEXT-L: #DEFINE_STREAM AAA 1

// CHECK-NEXT-L: MAIN_1:
main_label:

LAUNCH_MISSION subscript1.sc
LOAD_AND_LAUNCH_MISSION miss1.sc
REGISTER_STREAMED_SCRIPT stream1.sc

// CHECK-L: IS_GAME_VERSION_ORIGINAL
IS_GAME_VERSION_ORIGINAL
// CHECK-NEXT-L: GET_LABEL_POINTER @MAIN_1 &8
GET_LABEL_POINTER main_label v2

// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT
TERMINATE_THIS_SCRIPT


// subscript1.sc
// CHECK-NEXT-L: MAIN_2:
// CHECK-NEXT-L: GET_THIS_SCRIPT_STRUCT &12
// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT

// miss1.sc
// CHECK-NEXT-L: #MISSION_BLOCK_START 0
// CHECK-NEXT-L: SCRIPT_NAME 'MISS1'
// CHECK-NEXT-L: MISSION_0_1:
// CHECK-NEXT-L: IS_CAR_SIREN_ON 0i8
// CHECK-NEXT-L: GOSUB_IF_FALSE %MISSION_0_1
// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT
// CHECK-NEXT-L: #MISSION_BLOCK_END

// stream1.sc
// CHECK-NEXT-L: #STREAMED_BLOCK_START 0
// CHECK-NEXT-L: SCRIPT_NAME 'STREAM1'
// CHECK-NEXT-L: STREAM_0_1:
// CHECK-NEXT-L: IS_KEY_PRESSED 0i8
// CHECK-NEXT-L: GOSUB_IF_FALSE %STREAM_0_1
// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT
// CHECK-NEXT-L: #STREAMED_BLOCK_END
