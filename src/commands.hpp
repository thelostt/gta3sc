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
    Constant,
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
        EntityType entity_type;     /// Entity type of this argument. Zero means none.

        Arg()
        {}

        explicit Arg(ArgType type) :
            type(type), optional(false), allow_constant(true), allow_global_var(true), allow_local_var(true)
        {
        }

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
    std::map<std::string, EntityType> entities;

public:
    using alternator_pair = std::pair<decltype(commands)::const_iterator, decltype(commands)::const_iterator>;

protected:
    shared_ptr<Enum> enum_models;
    shared_ptr<Enum> enum_carpedmodels;

    optional<const Command&> cmd_SET_PROGRESS_TOTAL;
    optional<const Command&> cmd_SET_TOTAL_NUMBER_OF_MISSIONS;
    optional<const Command&> cmd_SET_COLLECTABLE1_TOTAL;
    optional<const Command&> cmd_SWITCH_START;
    optional<const Command&> cmd_SWITCH_CONTINUED;
    optional<const Command&> cmd_GOSUB_FILE;
    optional<const Command&> cmd_RETURN;
    optional<const Command&> cmd_LAUNCH_MISSION;
    optional<const Command&> cmd_LOAD_AND_LAUNCH_MISSION;
    optional<const Command&> cmd_START_NEW_SCRIPT;
    optional<const Command&> cmd_TERMINATE_THIS_SCRIPT;
    optional<const Command&> cmd_RET;
    optional<const Command&> cmd_GOTO;
    optional<const Command&> cmd_GOTO_IF_FALSE;
    optional<const Command&> cmd_ANDOR;
    alternator_pair          alt_SET;
    alternator_pair          alt_CSET;
    alternator_pair          alt_TERMINATE_THIS_CUSTOM_SCRIPT;
    alternator_pair          alt_ADD_THING_TO_THING;
    alternator_pair          alt_SUB_THING_FROM_THING;
    alternator_pair          alt_MULT_THING_BY_THING;
    alternator_pair          alt_DIV_THING_BY_THING;
    alternator_pair          alt_ADD_THING_TO_THING_TIMED;
    alternator_pair          alt_SUB_THING_FROM_THING_TIMED;
    alternator_pair          alt_IS_THING_EQUAL_TO_THING;
    alternator_pair          alt_IS_THING_GREATER_THAN_THING;
    alternator_pair          alt_IS_THING_GREATER_OR_EQUAL_TO_THING;


public:
    Commands(std::multimap<std::string, Command> commands,
             std::map<std::string, EntityType> entities,
             std::map<std::string, shared_ptr<Enum>> enums);

    Commands(const Commands&) = delete;

    Commands(Commands&&) = default;

    ///
    static Commands from_xml(const std::vector<fs::path>& xml_list);

    /// Adds the default models associated with the program context into the CARPEDMODEL enum.
    ///
    /// \warning This method is not thread-safe.
    void add_default_models(const std::map<std::string, uint32_t, iless>&);

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

    /// Finds the literal value of a constant 'value'.
    /// This version searches for *all* enums in order to allow CONST type arguments
    optional<int32_t> find_constant_all(const std::string& value) const;

    /// Finds the literal value of a constant `value` assuming we're dealing with argument `arg`.
    optional<int32_t> find_constant_for_arg(const std::string& value, const Command::Arg& arg) const;


    /// Find a command base on its name.
    optional<const Command&> find_command(const char* name) const
    {
        auto it = this->commands.find(name);
        if(it != this->commands.end())
            return it->second;
        return nullopt;
    }

    /// Finds a range of commands with the same alternator name.
    alternator_pair find_alternator(const char* name) const
    {
        return commands.equal_range(name);
    }

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

    bool equal(const Command& rhs, optional<const Command&> lhs) const
    {
        if(lhs) return equal(rhs, *lhs);
        return false;
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

    optional<std::string> find_entity_name(EntityType type) const
    {
        if(type == 0)
            return std::string("NONE");

        for(auto& pair : this->entities)
        {
            if(pair.second == type)
                return pair.first;
        }

        return nullopt;
    }

    bool is_alternator(const Command& command, alternator_pair alt) const
    {
        return this->equal(command, alt);
    }

    // --- Important Commands ---

    optional<const Command&> set_progress_total() const
    {
        return this->cmd_SET_PROGRESS_TOTAL;
    }

    optional<const Command&> set_total_number_of_missions() const
    {
        return this->cmd_SET_TOTAL_NUMBER_OF_MISSIONS;
    }

    optional<const Command&> set_collectable1_total() const
    {
        return this->cmd_SET_COLLECTABLE1_TOTAL;
    }

    optional<const Command&> switch_start() const
    {
        return this->cmd_SWITCH_START;
    }

    optional<const Command&> switch_continued() const
    {
        return this->cmd_SWITCH_CONTINUED;
    }

    optional<const Command&> gosub_file() const
    {
        return this->cmd_GOSUB_FILE;
    }

    optional<const Command&> launch_mission() const
    {
        return this->cmd_LAUNCH_MISSION;
    }

    optional<const Command&> load_and_launch_mission() const
    {
        return this->cmd_LOAD_AND_LAUNCH_MISSION;
    }

    optional<const Command&> start_new_script() const
    {
        return this->cmd_START_NEW_SCRIPT;
    }

    optional<const Command&> terminate_this_script() const
    {
        return this->cmd_TERMINATE_THIS_SCRIPT;
    }

    alternator_pair terminate_this_custom_script() const
    {
        return this->alt_TERMINATE_THIS_CUSTOM_SCRIPT;
    }

    optional<const Command&> return_() const    // can't be named purely return() because of the C keyword
    {
        return this->cmd_RETURN;
    }

    optional<const Command&> ret() const
    {
        return this->cmd_RET;
    }

    optional<const Command&> goto_() const    // can't be named purely goto() because of the C keyword
    {
        return this->cmd_GOTO;
    }

    optional<const Command&> goto_if_false() const
    {
        return this->cmd_GOTO_IF_FALSE;
    }

    optional<const Command&> andor() const
    {
        return this->cmd_ANDOR;
    }

    alternator_pair set() const
    {
        return this->alt_SET;
    }

    alternator_pair cset() const
    {
        return this->alt_CSET;
    }

    alternator_pair add_thing_to_thing() const
    {
        return this->alt_ADD_THING_TO_THING;
    }

    alternator_pair sub_thing_from_thing() const
    {
        return this->alt_SUB_THING_FROM_THING;
    }

    alternator_pair mult_thing_by_thing() const
    {
        return this->alt_MULT_THING_BY_THING;
    }

    alternator_pair div_thing_by_thing() const
    {
        return this->alt_DIV_THING_BY_THING;
    }

    alternator_pair add_thing_to_thing_timed() const
    {
        return this->alt_ADD_THING_TO_THING_TIMED;
    }

    alternator_pair sub_thing_from_thing_timed() const
    {
        return this->alt_SUB_THING_FROM_THING_TIMED;
    }

    alternator_pair is_thing_equal_to_thing() const
    {
        return this->alt_IS_THING_EQUAL_TO_THING;
    }

    alternator_pair is_thing_greater_than_thing() const
    {
        return this->alt_IS_THING_GREATER_THAN_THING;
    }

    alternator_pair is_thing_greater_or_equal_to_thing() const
    {
        return this->alt_IS_THING_GREATER_OR_EQUAL_TO_THING;
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
