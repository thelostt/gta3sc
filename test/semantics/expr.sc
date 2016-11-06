// Tests the semantics of expressions
// RUN: %dis %gta3sc %s --config=gta3 -fsyntax-only 2>&1 | %verify %s

VAR_INT   i j k
VAR_FLOAT x y z

// Test non-commutative operations
i = 2 * i
i = i * 2
i = 2 + i
i = i + 2
i = 2 / i   // expected-error {{cannot do VAR1 = THING / VAR1}}
i = i / 2
i = 2 - i   // expected-error {{cannot do VAR1 = THING - VAR1}}
i = i - 2
x = 2.0 +@ x  // expected-error {{cannot do VAR1 = THING +@ VAR1}} 
x = x +@ 2.0
x = 2.0 -@ x  // expected-error {{cannot do VAR1 = THING -@ VAR1}} 
x = x -@ 2.0
// ...except that those are valid...
x = x - x
x = x / x
x = x +@ x
x = x -@ x

// Increment/Decrement only works with integers
++i
++x // expected-error {{increment operand must be of type INT}}

TERMINATE_THIS_SCRIPT