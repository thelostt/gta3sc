// Tests the codegeneration of expressions.
// RUN: %gta3sc %s --config=gta3 -mno-header --guesser -o%t && %checksum %t ec8bd3d3b39ba4f7ecfa9d99d5b01c10

VAR_INT   i j k
VAR_FLOAT x y z

// Test commutation property
i = 2 * i // compiled as i *= 2
i = i * 2 // compiled as i *= 2
i = 2 + i // compiled as i += 2
i = i + 2 // compiled as i += 2

TERMINATE_THIS_SCRIPT