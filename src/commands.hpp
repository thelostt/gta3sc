#pragma once
#include "stdinc.h"

enum class ArgType : uint8_t
{
    Any,
    Label,
    Buffer128,
    Integer,
    Float,
    TextLabel,
};

bool argtype_matches(ArgType type1, ArgType type2);

struct Command
{
    struct Arg
    {
        ArgType type;
        bool optional : 1;
        bool allow_constant : 1;
        bool allow_local_var : 1;
        bool allow_global_var : 1;
        // std::vector<shared_ptr<Enum>> enums;
    };

    bool                supported;
    uint16_t            id;
    //std::array<Arg, 40> args;
    std::vector<Arg>    args;

    bool has_optional() const
    {
        return args.empty()? false : args.back().optional;
    }
};

struct Commands
{
    std::multimap<std::string, Command> commands;

    // throws BadAlternator on error
    const Command& match(const SyntaxTree& command_node) const;
};

inline
const Command& Commands::match(const SyntaxTree& command_node) const
{
    auto num_args = command_node.child_count() - 1;
    auto alter_range = commands.equal_range(command_node.child(0).text());

    for(auto alter_kv = alter_range.first; alter_kv != alter_range.second; ++alter_kv)
    {
        size_t arg_readen = 0;

        const Command& alter = alter_kv->second;

        auto it_alt_arg = alter.args.begin();
        auto it_target_arg = command_node.begin() + 1;

        bool is_optional = false;

        for(; ;
        (it_alt_arg->optional? it_alt_arg : ++it_alt_arg),
            ++it_target_arg,
            ++arg_readen)
        {
            assert(arg_readen <= num_args);

            if(arg_readen < num_args)
            {
                if(it_alt_arg == alter.args.end())
                    break;
            }
            else // arg_readen == num_args, i.e. end of arguments
            {
                if(it_alt_arg == alter.args.end() || it_alt_arg->optional)
                    return alter;
                else
                    break;
            }

            bool bad_alternative = false;

            switch((*it_target_arg)->type())
            {
                case NodeType::Integer:
                    bad_alternative = (!argtype_matches(it_alt_arg->type, ArgType::Integer) || !it_alt_arg->allow_constant);
                    break;
                case NodeType::Float:
                    bad_alternative = (!argtype_matches(it_alt_arg->type, ArgType::Float) || !it_alt_arg->allow_constant);
                    break;
                case NodeType::Array:
                    // TODO
                    break;
                case NodeType::Identifier:
                    // TODO
                    break;
                case NodeType::ShortString:
                    bad_alternative = (!argtype_matches(it_alt_arg->type, ArgType::TextLabel) || !it_alt_arg->allow_constant);
                case NodeType::LongString:
                    bad_alternative = (!argtype_matches(it_alt_arg->type, ArgType::TextLabel) && !argtype_matches(it_alt_arg->type, ArgType::Buffer128));
                    bad_alternative = bad_alternative || !it_alt_arg->allow_constant;;
                    break;
            }

            if(bad_alternative)
            {
                // try another alternative
                break;
            }
        }
    }

    throw BadAlternator("TODO");
}

inline
Commands get_test_commands()
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

inline bool argtype_matches(ArgType type1, ArgType type2)
{
    return type1 == type2 || type1 == ArgType::Any || type2 == ArgType::Any;
}

