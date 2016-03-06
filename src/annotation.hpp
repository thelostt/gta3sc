#pragma once
#include "stdinc.h"

struct RepeatAnnotation
{
    const Command& set_var_to_zero;
    const Command& add_var_with_one;
    const Command& is_var_geq_times;
    // numbers compatible with set_var_to_times and add_var_with_int
    shared_ptr<SyntaxTree> number_zero;
    shared_ptr<SyntaxTree> number_one;
};
