// Tests the SWITCH semantics
// RUN: %dis %gta3sc %s --config=gta3 -fsyntax-only -fswitch -pedantic --guesser 2>&1 | %verify %s
VAR_INT n j

SWITCH n
    CASE 1      // expected-error {{CASE does not end with a BREAK}}
    CASE 1      ////// TODO expected-error {{CASE happens twice}}
        BREAK   // TODO FIXME ^ (both above)
    CASE 2      // expected-error {{CASE does not end with a BREAK}}
        WAIT 0  
    CASE 3
        BREAK
    CASE j      // expected-error {{case value must be a integer constant}}
        BREAK
    CASE 1.0    // expected-error@5 {{BAD ALTERNATOR}}
        BREAK   // TODO FIXME ^ should be expected int constant
ENDSWITCH

TERMINATE_THIS_SCRIPT