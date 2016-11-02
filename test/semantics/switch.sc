// Tests the SWITCH semantics
// RUN: %dis %gta3sc %s --config=gta3 -fsyntax-only -fswitch -pedantic --guesser 2>&1 | %verify %s
VAR_INT n j

// TODO test more than 75 cases switch (should error), not sure if here in test/semantics

SWITCH n
    CASE 1
    CASE 1      // expected-error {{duplicate CASE}}
        BREAK
    CASE 2      // expected-error {{does not end with a BREAK}}
        WAIT 0  
    CASE 3
        BREAK
    CASE j      // expected-error {{must be a integer constant}}
        BREAK
    CASE 1.0    // expected-error {{type mismatch}}
        BREAK
ENDSWITCH

TERMINATE_THIS_SCRIPT