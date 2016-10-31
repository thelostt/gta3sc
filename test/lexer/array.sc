// RUN: %dis %gta3sc %s --config=gtasa --guesser -fsyntax-only 2>&1 | %verify %s

VAR_INT a[10] b[10] i

WAIT a[1]
WAIT a[1]works   // same as WAIT a[1]
WAIT a[b[1]]     // expected-error {{GIVE ME A ERROR MESSAGE}}
WAIT a[b]        // expected-error {{GIVE ME A ERROR MESSAGE}}
WAIT a[i]

TERMINATE_THIS_SCRIPT