#pragma once
#include "stdinc.h"

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
    std::map<std::string, int32_t, iless> values;
    bool is_global = false;

    Enum(std::map<std::string, int32_t, iless> values, bool is_global) :
        values(std::move(values)), is_global(is_global)
    {}

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

        /// Finds a constant associated with the enums of this argument.
        ::optional<int32_t> find_constant(const std::string& value) const
        {
            for(auto& e : enums)
            {
                if(auto opt = e->find(value))
                    return opt;
            }
            return nullopt;
        }

        /// Checks whether this argument uses the specified enum.
        bool uses_enum(const shared_ptr<Enum>& e) const 
        {
            if(!e) return false;
            return std::find(enums.begin(), enums.end(), e) != enums.end();
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
protected:
    std::multimap<std::string, Command> commands;
    std::multimap<uint16_t, const Command*> commands_by_id; // TODO use shared_ptr<Command>!?
    std::map<std::string, shared_ptr<Enum>> enums;

    shared_ptr<Enum> enum_models;
    shared_ptr<Enum> enum_carpedmodels;

public:
    using alternator_pair = std::pair<decltype(commands)::const_iterator, decltype(commands)::const_iterator>;

    Commands(std::multimap<std::string, Command> commands,
             std::map<std::string, shared_ptr<Enum>> enums);

    ///
    static Commands from_xml(const std::vector<fs::path>& xml_list);

    /// Adds the default models associated with the program context into the CARPEDMODEL enum.
    ///
    /// \warning This method is not thread-safe.
    void add_default_models(const ProgramContext& program);

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
    ///
    /// If the argument is a model enum, and the identifier is unknown, a model gets appened to the `script` model table.
    void annotate(SyntaxTree& command_node, const Command& command, const SymTable&, const shared_ptr<Scope>&, Script&, ProgramContext&) const;

    /// Annotates the argument `nodes...` for a specific `command`.
    /// If nodes are already annotated, will just ensure the type of annotation is the same  (program will abort otherwise).
    ///
    ///  If the argument is a model enum, and the identifier is unknown, a model gets appened to the `script` model table.
    template<typename... TSyntaxTree>
    void annotate_args(const SymTable& symbols, const shared_ptr<Scope>& scope, Script& s, ProgramContext& p, const Command& command, TSyntaxTree&... nodes) const
    {
        SyntaxTree* args[] = { std::addressof<TSyntaxTree>(nodes)... };
        return annotate_internal(symbols, scope, s, p, command, std::begin(args), std::end(args));
    }

    /// Finds the literal value of a constant `value`.
    /// `context_free_only` is whether we only search for constants that can be used in any occasion or
    /// constants that can be used only in specific commands arguments.
    optional<int32_t> find_constant(const std::string& value, bool context_free_only) const;

    /// Finds the literal value of a constant `value` assuming we're dealing with argument `arg`.
    optional<int32_t> find_constant_for_arg(const std::string& value, const Command::Arg& arg) const;


    /// Find a command based on its id.
    optional<const Command&> find_command(uint16_t id) const
    {
        auto it = this->commands_by_id.find(id);
        if(it != this->commands_by_id.end())
            return *it->second;
        return nullopt;
    }

    optional<std::string> find_command_name(uint16_t id, bool never_fail = false) const
    {
        // TODO speed up this search

        for(auto& pair : this->commands)
        {
            if(pair.second.id == id)
                return pair.first;
        }

        if(never_fail)
        {
            char buffer[sizeof("COMMAND_")-1 + 4 + 1];
            snprintf(buffer, sizeof(buffer), "COMMAND_%.4X", id);
            return std::string(std::begin(buffer), std::end(buffer) - 1);
        }

        return nullopt;
    }

    bool equal(const Command& rhs, const Command& lhs) const
    {
        return &rhs == &lhs;
    }

    bool equal(const Command& rhs, alternator_pair lhs) const
    {
        for(auto it = lhs.first; it != lhs.second; ++it)
        {
            if(this->equal(rhs, it->second))
                return true;
        }
        return false;
    }

    const shared_ptr<Enum>& get_models_enum() const
    {
        return this->enum_models;
    }


    // --- Important Commands ---

    const Command& script_name() const
    {
        // TODO cached
        return commands.find("SCRIPT_NAME")->second;
    }

    const Command& set_progress_total() const
    {
        // TODO cached
        return commands.find("SET_PROGRESS_TOTAL")->second;
    }

    const Command& set_total_number_of_missions() const
    {
        // TODO cached
        return commands.find("SET_TOTAL_NUMBER_OF_MISSIONS")->second;
    }

    const Command& set_collectable1_total() const
    {
        // TODO cached
        return commands.find("SET_COLLECTABLE1_TOTAL")->second;
    }

    const Command& switch_start() const
    {
        // TODO cached
        return commands.find("SWITCH_START")->second;
    }

    const Command& switch_continued() const
    {
        // TODO cached
        return commands.find("SWITCH_CONTINUED")->second;
    }

    const Command& gosub_file() const
    {
        // TODO cached
        return commands.find("GOSUB_FILE")->second;
    }

    const Command& launch_mission() const
    {
        // TODO cached
        return commands.find("LAUNCH_MISSION")->second;
    }

    const Command& load_and_launch_mission() const
    {
        // TODO cached
        return commands.find("LOAD_AND_LAUNCH_MISSION")->second;
    }

    const Command& start_new_script() const
    {
        // TODO cached
        return commands.find("START_NEW_SCRIPT")->second;
    }

    const Command& terminate_this_script() const
    {
        // TODO cached
        return commands.find("TERMINATE_THIS_SCRIPT")->second;
    }

    alternator_pair terminate_this_custom_script() const
    {
        return commands.equal_range("TERMINATE_THIS_CUSTOM_SCRIPT");
    }

    const Command& return_() const    // can't be named purely return() because of the C keyword
    {
        // TODO cached
        return commands.find("RETURN")->second;
    }

    const Command& ret() const
    {
        // TODO cached
        return commands.find("RET")->second;
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

protected:

private:

    const Command& match_internal(const SymTable&, const shared_ptr<Scope>&,
        alternator_pair commands, const SyntaxTree** begin, const SyntaxTree** end) const;

    void annotate_internal(const SymTable&, const shared_ptr<Scope>&, Script&, ProgramContext&,
        const Command&, SyntaxTree** begin, SyntaxTree** end) const;
};

/// Checks if the argument types are compatible with each other.
inline bool argtype_matches(ArgType type1, ArgType type2)
{
    return type1 == type2 || type1 == ArgType::Any || type2 == ArgType::Any;
}

/*
inline shared_ptr<Enum> make_enum(std::map<std::string, int32_t> init)
{
    return std::make_shared<Enum>(Enum { std::move(init), false });
}
*/
