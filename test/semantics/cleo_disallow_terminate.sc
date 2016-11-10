// RUN: %dis %gta3sc %s --config=gtasa --guesser --cs -fsyntax-only 2>&1 | 
{
TERMINATE_THIS_SCRIPT // expected-error {{this command is not allowed in custom scripts}}
}