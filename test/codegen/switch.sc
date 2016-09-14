// Tests the SWITCH code generation.
// RUN: %gta3sc %s --config=gta3 -mno-header -fswitch --guesser -o%t3.cs && %checksum %t3.cs 0c9fe4b044b782d50d873b83a13c59a8
// RUN: %gta3sc %s --config=gtasa -mno-header -fswitch --guesser -o%tsa.cs && %checksum %tsa.cs e2f220804930cbc2090e7c59d0968ec9
VAR_INT n

// Using a default case
SWITCH n
    CASE 100
        WAIT 100
        BREAK
    CASE 200
        WAIT 200
        BREAK
    CASE 300
        WAIT 300
        BREAK
    CASE 50         // Out of order
        WAIT 50
        BREAK
    DEFAULT
        WAIT 0
        BREAK
ENDSWITCH

// Using no default case
SWITCH n
    CASE 100
        WAIT 100
        BREAK
    CASE 200
        WAIT 200
        BREAK
    CASE 50
        WAIT 50
        BREAK
ENDSWITCH

// Using over 7 cases
SWITCH n
    CASE 100
        WAIT 100
        BREAK
    CASE 200
        WAIT 200
        BREAK
    CASE 300
        WAIT 300
        BREAK
    CASE 400
        WAIT 400
        BREAK
    CASE 500
        WAIT 500
        BREAK
    CASE 600
        WAIT 600
        BREAK
    CASE 700
        WAIT 700
        BREAK
    CASE 800
        WAIT 800
        BREAK
    CASE 900
        WAIT 900
        BREAK
ENDSWITCH

// TODO test more than 75 cases switch (should error)

TERMINATE_THIS_SCRIPT