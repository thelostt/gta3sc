#pragma once
#include <stdinc.h>
#include "commands.hpp"

struct TextLabelAnnotation
{
    bool        is_varlen;
    bool        preserve_case;
    std::string string;
};

struct String128Annotation
{
    std::string string;
};

// not used
struct VarAnnotation
{
    shared_ptr<Var>                             base;
    optional<variant<int32_t, shared_ptr<Var>>> index; // int32_t index is 0-based
};

struct ArrayAnnotation
{
    shared_ptr<Var>                  base;
    variant<int32_t, shared_ptr<Var>> index;    // int32_t index is 0-based
};

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
    Commands::MatchArgument number_zero;
    Commands::MatchArgument number_one;
};

struct SwitchAnnotation
{
    size_t num_cases;
    bool   has_default;
};

struct SwitchCaseAnnotation
{
    const Command* is_var_eq_int; // always a valid pointer
    // (cannot use const Command& because of std::any requiring a copy ctor for stack allocated objects)
};

struct IncDecAnnotation
{
    const Command& op_var_with_one;
    Commands::MatchArgument number_one;
};

// Hack
struct StreamedFileAnnotation
{
    int32_t id;
};

// Instead of a const Command&, annotate this
struct CommandSkipCutsceneStartAnnotation
{
};

// Instead of a const Command&, annotate this
struct CommandSkipCutsceneEndAnnotation
{
};


// TODO move those functions to somewhere else?

inline string_view remove_quotes(const string_view& string)
{
    Expects(string.size() >= 2 && string.front() == '"' && string.back() == '"');
    return string_view(string.data() + 1, string.size() - 2);
}
