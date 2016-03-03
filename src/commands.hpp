#pragma once
#include "stdinc.h"
#include "parser.hpp"

struct SymTable;
struct Scope;

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

/// Stores constant values associated with a identifier.
struct Enum
{
    // TODO pendantically, only ints are allowed, but maybe later we can add floats and strings.
    std::map<std::string, int32_t> values;

    optional<int32_t> find(const std::string& value) const
    {
        auto it = values.find(value);
        if(it != values.end())
            return it->second;
        return nullopt;
    }
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
        std::vector<shared_ptr<Enum>> enums;

        ::optional<int32_t> find_constant(const std::string& value) const
        {
            for(auto& e : enums)
            {
                if(auto opt = e->find(value))
                    return opt;
            }
            return nullopt;
        }
    };

    // TODO maybe bool conditional?
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
    std::map<std::string, shared_ptr<Enum>> enums; // [""] stores enums allowed on every context

    /// Matches the best command based on the name and arguments given a COMMAND node in the AST.
    ///
    /// \throws `BadAlternator` if no match found.
    const Command& match(const SyntaxTree& command_node, const SymTable&, const shared_ptr<Scope>&) const;

    /// TODO doc
    void annotate(SyntaxTree& command_node, const Command&, const SymTable&, const shared_ptr<Scope>&) const;

    optional<int32_t> find_constant(const std::string& value, bool context_free_only) const
    {
        if(context_free_only)
        {
            auto it = enums.find("");
            if(it != enums.end())
            {
                if(it->second != nullptr)
                    return it->second->find(value);
            }
            return nullopt;
        }
        else
        {
            // TODO
            return nullopt;
        }
    }

    optional<int32_t> find_constant_for_arg(const std::string& value, const Command::Arg& arg) const
    {
        if(auto opt_const = arg.find_constant(value))
            return opt_const;
        else if(auto opt_const = this->find_constant(value, true))
            return opt_const;
        return nullopt;
    }

    const Command& goto_() const    // can't be named purely goto() because of the C keyword
    {
        // TODO cached
        return commands.find("GOTO")->second;
    }

    const Command& goto_if_false() const
    {
        // TODO cached
        return commands.find("GOTO_IF_FALSE")->second;
    }

    const Command& andor() const
    {
        // TODO cached
        return commands.find("ANDOR")->second;
    }
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
                    { ArgType::Integer, false, true, true, true,
                    { std::make_shared<Enum>(Enum { { {"TEST1", 1111}, {"TEST2", 2222} } }) } },
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
        {
            "GOTO_IF_FALSE",
            {
                true,
                0x004D,
                {
                    { ArgType::Label, false, true, true, true, },
                },
            }
        },
        {
            "ANDOR",
            {
                true,
                0x00D6,
                {
                    { ArgType::Integer, false, true, false, false, },
                },
            }
        },
      },
      {
          {
              "", std::make_shared<Enum>(Enum {{
                  {"TRUE", 1}, {"FALSE", 0}, // TODO DAY NIGHT 
                }})
          }
      },
    };
}

/// Checks if the argument types are compatible with each other.
inline bool argtype_matches(ArgType type1, ArgType type2)
{
    return type1 == type2 || type1 == ArgType::Any || type2 == ArgType::Any;
}
