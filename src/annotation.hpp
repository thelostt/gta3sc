#pragma once
#include "stdinc.h"

struct ModelAnnotation
{
    weak_ptr<const Script> where;       // weak to avoid circular reference
    uint32_t               id;
};

struct RepeatAnnotation
{
    const Command& set_var_to_zero;
    const Command& add_var_with_one;
    const Command& is_var_geq_times;
    // numbers compatible with set_var_to_times and add_var_with_int
    shared_ptr<SyntaxTree> number_zero;
    shared_ptr<SyntaxTree> number_one;
};

struct SwitchCaseAnnotation
{
    const Command* is_var_eq_int; // always a valid pointer
    // (cannot use const Command& because of std::any requiring a copy ctor for stack allocated objects)
};

struct SwitchCaseBreakAnnotation
{
    // annotated on the last statement (if BREAK) of a CASE/DEFAULT
};

struct IncDecAnnotation
{
    const Command& op_var_with_one;
    shared_ptr<SyntaxTree> number_one;
};
