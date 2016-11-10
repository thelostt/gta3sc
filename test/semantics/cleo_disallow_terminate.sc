// RUN: %dis %gta3sc %s --config=gtasa --guesser --cs -fsyntax-only 2>&1 | %verify %s
{
TERMINATE_THIS_SCRIPT // expected-error {{command not allowed in custom scripts, please use TERMINATE_THIS_CUSTOM_SCRIPT}}
}