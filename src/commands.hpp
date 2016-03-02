#pragma once
#include "stdinc.h"
#include "parser.hpp"

/// Fundamental type of a command argument.
enum class ArgType : uint8_t
{
    Any,
    Label,
    Buffer128,
    Integer,
    Float,
    TextLabel,
};

/// Stores command information.
struct Command
{
    struct Arg
    {
        ArgType type;               /// Fundamental type of argument.
        bool optional : 1;          /// Allows as many optional arguments as possible (must be the last arg).
        bool allow_constant : 1;    /// Allow literal values
        bool allow_local_var : 1;   /// Allow local variables
        bool allow_global_var : 1;  /// Allow global variables
        //std::vector<shared_ptr<Enum>> enums;
    };

    bool                supported;  /// Is this command supported by the script engine?
    uint16_t            id;         /// The opcode id.
    //std::array<Arg, 40> args;
    std::vector<Arg>    args;       /// The arguments of the command.

    /// Checks if there's any optional argument on this command.
    bool has_optional() const
    {
        return args.empty()? false : args.back().optional;
    }
};

/// Stores the list of commands and alternators.
struct Commands
{
    std::multimap<std::string, Command> commands;

    /// Matches the best command based on the name and arguments given a COMMAND node in the AST.
    ///
    /// \throws `BadAlternator` if no match found.
    const Command& match(const SyntaxTree& command_node) const;
};

inline Commands get_test_commands()
{
    return Commands
    { {
        {
            "WAIT",
            {
                true,
                0x0001,
                {
                    { ArgType::Integer, false, true, true, true, },
                },
            }
        },
        {
            "GOTO",
            {
                true,
                0x0002,
                {
                    { ArgType::Label, false, true, true, true, },
                },
            }
        },
        } };
}

/// Checks if the argument types are compatible with each other.
inline bool argtype_matches(ArgType type1, ArgType type2)
{
    return type1 == type2 || type1 == ArgType::Any || type2 == ArgType::Any;
}
