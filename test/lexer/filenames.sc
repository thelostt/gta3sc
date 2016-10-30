// RUN: %dis %gta3sc %s --config=gta3 -fsyntax-only 2>&1 | %verify %s

LAUNCH_MISSION _a.sc    // valid! invalid identifier, yet valid filename.
TERMINATE_THIS_SCRIPT

// expected-error@_a.sc:2 {{invalid identifier}}