#pragma once
#include "stdinc.h"

class SyntaxTree;
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
        bool allow_global_var : 1;  /// Allow global variables
        bool allow_local_var : 1;   /// Allow local variables
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
private:
    std::multimap<std::string, Command> commands;
    std::map<std::string, shared_ptr<Enum>> enums; // [""] stores enums allowed on every context

public:
    using alternator_pair = std::pair<decltype(commands)::const_iterator, decltype(commands)::const_iterator>;

    Commands(std::initializer_list<decltype(commands)::value_type> init_cmds,
             std::initializer_list<decltype(enums)::value_type> init_enums) :
        commands(std::move(init_cmds)), enums(std::move(init_enums))
    {
    }

    /// Matches the best command based on the alternators with the command name and arguments given a COMMAND node in the AST.
    ///
    /// \throws `BadAlternator` if no match found.
    const Command& match(const SyntaxTree& command_node, const SymTable&, const shared_ptr<Scope>&) const;

    /// Matches the best command based on the given alternators (`commands`) and its arguments in the AST `nodes...`.
    ///
    /// \throws `BadAlternator` if no match found.
    template<typename... TSyntaxTree>
    const Command& match_args(const SymTable& symbols, const shared_ptr<Scope>& scope, alternator_pair commands, const TSyntaxTree&... nodes) const
    {
        const SyntaxTree* args[] = { std::addressof<const TSyntaxTree>(nodes)... };
        return match_internal(symbols, scope, commands, std::begin(args), std::end(args));
    }

    /// Annotates the argument nodes of a COMMAND node in the AST for a specific `command`.
    /// If nodes are already annotated, will just ensure the type of annotation is the same (program will abort otherwise).
    void annotate(SyntaxTree& command_node, const Command& command, const SymTable&, const shared_ptr<Scope>&) const;

    /// Annotates the argument `nodes...` for a specific `command`.
    /// If nodes are already annotated, will just ensure the type of annotation is the same  (program will abort otherwise).
    template<typename... TSyntaxTree>
    void annotate_args(const SymTable& symbols, const shared_ptr<Scope>& scope, const Command& command, TSyntaxTree&... nodes) const
    {
        SyntaxTree* args[] = { std::addressof<TSyntaxTree>(nodes)... };
        return annotate_internal(symbols, scope, command, std::begin(args), std::end(args));
    }

    /// Finds the literal value of a constant `value`.
    /// `context_free_only` is whether we only search for constants that can be used in any occasion or
    /// constants that can be used only in specific commands arguments.
    optional<int32_t> find_constant(const std::string& value, bool context_free_only) const;

    /// Finds the literal value of a constant `value` assuming we're dealing with argument `arg`.
    optional<int32_t> find_constant_for_arg(const std::string& value, const Command::Arg& arg) const;


    // --- Important Commands ---

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

    alternator_pair set() const
    {
        // TODO cached
        return commands.equal_range("SET");
    }

    alternator_pair cset() const
    {
        // TODO cached
        return commands.equal_range("CSET");
    }

    alternator_pair add_thing_to_thing() const
    {
        // TODO cached
        return commands.equal_range("ADD_THING_TO_THING");
    }

    alternator_pair sub_thing_from_thing() const
    {
        // TODO cached
        return commands.equal_range("SUB_THING_FROM_THING");
    }

    alternator_pair mult_thing_by_thing() const
    {
        // TODO cached
        return commands.equal_range("MULT_THING_BY_THING");
    }

    alternator_pair div_thing_by_thing() const
    {
        // TODO cached
        return commands.equal_range("DIV_THING_BY_THING");
    }

    alternator_pair add_thing_to_thing_timed() const
    {
        // TODO cached
        return commands.equal_range("ADD_THING_TO_THING_TIMED");
    }

    alternator_pair sub_thing_from_thing_timed() const
    {
        // TODO cached
        return commands.equal_range("SUB_THING_FROM_THING_TIMED");
    }

    alternator_pair is_thing_equal_to_thing() const
    {
        // TODO cached
        return commands.equal_range("IS_THING_EQUAL_TO_THING");
    }

    alternator_pair is_thing_greater_than_thing() const
    {
        // TODO cached
        return commands.equal_range("IS_THING_GREATER_THAN_THING");
    }

    alternator_pair is_thing_greater_or_equal_to_thing() const
    {
        // TODO cached
        return commands.equal_range("IS_THING_GREATER_OR_EQUAL_TO_THING");
    }

private:

    const Command& match_internal(const SymTable&, const shared_ptr<Scope>&,
        alternator_pair commands, const SyntaxTree** begin, const SyntaxTree** end) const;

    void annotate_internal(const SymTable&, const shared_ptr<Scope>&,
        const Command&, SyntaxTree** begin, SyntaxTree** end) const;
};

/// Checks if the argument types are compatible with each other.
inline bool argtype_matches(ArgType type1, ArgType type2)
{
    return type1 == type2 || type1 == ArgType::Any || type2 == ArgType::Any;
}
