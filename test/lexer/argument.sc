// RUN: %dis %gta3sc %s --config=gtasa --guesser -fsyntax-only 2>&1 | %verify %s

VAR_TEXT_LABEL ab3

// Integer Tokens
WAIT 1
WAIT -1
WAIT 5-2 // valid, same as 5

// Float Tokens
SET_TIME_SCALE .0
SET_TIME_SCALE 1.0
SET_TIME_SCALE 1.0f
SET_TIME_SCALE 1.0F
SET_TIME_SCALE -1.0
SET_TIME_SCALE 1.0.9 // valid, same as 1.0
SET_TIME_SCALE 1.0f9 // valid, same as 1.0
SET_TIME_SCALE 1.0-9 // expected-error {{invalid numeric literal}}

// Identifier Tokens
PRINT_HELP ab3
PRINT_HELP $ab3
PRINT_HELP _ab3   // expected-error {{invalid identifier}}
PRINT_HELP ab3^?_ // valid
PRINT_HELP WHILE  // valid. identifier, not a keyword.

TERMINATE_THIS_SCRIPT