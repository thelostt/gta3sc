#include "commands.hpp"
#include "error.hpp"

/// Finds all commands named after a specific name (found in the syntax tree node),
/// and iterates on the arguments of the syntax tree node and each of these commands
/// until one of them finally matches the content of the syntax tree arguments.
///
/// Due to giving a descriptive error message, it throws BadAlternator instead of
/// returning a nullopt when a command cannot be matched.
const Command& Commands::match(const SyntaxTree& command_node) const
{
    auto num_target_args  = command_node.child_count() - 1;
    auto alternator_range = commands.equal_range(command_node.child(0).text());

    for(auto it = alternator_range.first; it != alternator_range.second; ++it)
    {
        const Command& alternative = it->second;

        bool is_optional = false;
        size_t args_readen = 0;

        auto it_alter_arg = alternative.args.begin();
        auto it_target_arg = command_node.begin() + 1;

        for( ; ;
            (it_alter_arg->optional? it_alter_arg : ++it_alter_arg),
            ++it_target_arg,
            ++args_readen)
        {
            assert(args_readen <= num_target_args);

            if(args_readen < num_target_args)
            {
                if(it_alter_arg == alternative.args.end())
                    break; // too many args on target
            }
            else // end of arguments
            {
                if(it_alter_arg == alternative.args.end() || it_alter_arg->optional)
                    return alternative;
                else
                    break; // too few args on target
            }

            bool bad_alternative = false;

            switch((*it_target_arg)->type())
            {
                case NodeType::Integer:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::Integer) && it_alter_arg->allow_constant);
                    break;
                case NodeType::Float:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::Float) && it_alter_arg->allow_constant);
                    break;
                case NodeType::Array:
                    // TODO
                    break;
                case NodeType::Identifier:
                    // TODO
                    break;
                case NodeType::ShortString:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::TextLabel) && it_alter_arg->allow_constant);
                case NodeType::LongString:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::TextLabel) || argtype_matches(it_alter_arg->type, ArgType::Buffer128));
                    bad_alternative = bad_alternative || !it_alter_arg->allow_constant;;
                    break;
            }

            if(bad_alternative)
                break; // try another alternative
        }
    }

    throw BadAlternator("TODO");
}
