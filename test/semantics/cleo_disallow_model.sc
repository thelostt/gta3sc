// RUN: %dis %gta3sc %s --config=gtasa --guesser --cs -fsyntax-only 2>&1 | %verify %s
{
LVAR_INT x
CREATE_OBJECT RANDOM_MODEL .0 .0 .0 x // expected-error@gta3sc:0 {{use of non-default model RANDOM_MODEL in custom script}}
TERMINATE_THIS_CUSTOM_SCRIPT
}