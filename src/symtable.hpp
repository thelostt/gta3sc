#pragma once
#include <stdinc.h>
#include "annotation.hpp"
#include "parser.hpp"
#include "script.hpp"

/// Stores scripts inclusion information.
class IncluderTable
{
public:
    /// Construts a IncluderTable from the lines in `script`.
    static IncluderTable from_script(const Script& script, ProgramContext& program);

    /// Merges includer table `t2` into this one.
    /// \warning this method is not exactly thread-safe.
    void merge(IncluderTable&& t2, ProgramContext& program);

    /// Scans all included scripts in the `script->tree` and adds them to this table.
    /// \warning this method is not exactly thread safe.
    void scan_for_includers(const Script& script, ProgramContext& program);

    /// Gets script type from filename.
    optional<ScriptType> script_type(const string_view& filename) const;

protected:
    friend class Script;
    bool add_script(ScriptType type, const SyntaxTree& command, ProgramContext& program);

public:
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    // IMPORTANT! Make sure whenever you add any new field to this object, to update merge() accordingly !!!!!!!!//
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    std::vector<std::string>    required;   //< REQUIRE scripts
    std::vector<std::string>    extfiles;   //< GOSUB_FILE scripts
    std::vector<std::string>    subscript;  //< LAUNCH_MISSION scripts
    std::vector<std::string>    mission;    //< LOAD_AND_LAUNCH_MISSION scripts
    std::vector<std::string>    streamed;   //< Streamed scripts
    std::vector<std::string>    streamed_names;//< Constant names associated with streamed scripts.
};

/// Stores important symbols defined throught scripts (labels, vars, scopes).
class SymTable
{
public:
    explicit SymTable()
    {}

    explicit SymTable(IncluderTable&& ictable) :
        ictable(std::move(ictable))
    {}

    /// Merges symbol table `t2` into this one.
    /// \warning this method is not exactly thread-safe.
    void merge(SymTable&& t2, ProgramContext& program);

    /// Construts a SymTable from the symbols in `script`.
    static SymTable from_script(Script& script, ProgramContext& program);

    /// Scans all symbols in `script->tree` and adds them to this table.
    // \warning this method is not exactly thread safe.
    void scan_symbols(Script& script, ProgramContext& program);

    /// Copies the list of scripts into this symbol table.
    /// \warning this method is not exactly thread safe.
    void build_script_table(const std::vector<shared_ptr<Script>>& scripts);

    /// \returns the highest global variable (based on index), or `nullopt` if no variable in this table.
    optional<shared_ptr<Var>> highest_global_var() const;

    /// \returns the size (in bytes) of the space required to store the global variables of this table.
    size_t size_global_vars() const;

    /// Shifts all global variable by `indices`.
    /// \warning this method is not exactly thread-safe.
    void apply_offset_to_vars(uint32_t indices);

    /// \returns the variable `name` (either global or local within `current_scope`).
    /// \note `current_scope` may be nullptr for no scope, otherwise it must be a scope owned by this table.
    optional<shared_ptr<Var>> find_var(const string_view& name, const shared_ptr<Scope>& current_scope) const;

    /// Finds the specified label in this table.
    optional<shared_ptr<Label>> find_label(const string_view& name) const;

    /// Finds the specified script in this table.
    optional<shared_ptr<Script>> find_script(const string_view& filename) const;

    /// Finds the specified streamed script id from its string constant.
    optional<uint16_t> find_streamed_id(const string_view& stream_constant) const;

    /// Checks whether local variables in scopes collides with global variables.
    void check_scope_collisions(ProgramContext& program) const;

    /// Checks whether variables collides with string constants.
    void check_constant_collisions(ProgramContext& program) const;

protected:
    friend class Script;

    bool add_script(ScriptType type, const SyntaxTree& command, ProgramContext& program);

    shared_ptr<Scope> add_scope(SyntaxTree& tree)
    {
        return *local_scopes.emplace(local_scopes.end(), new Scope(tree.shared_from_this()));
    }

    shared_ptr<Label> add_label(const shared_ptr<const SyntaxTree>& node, shared_ptr<const Scope> scope, shared_ptr<const Script> script)
    {
        auto it = this->labels.emplace(node->text().to_string(), std::make_shared<Label>(node, scope, script));
        if(it.second == false)
            return nullptr;
        return it.first->second;
    }

public:
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    // IMPORTANT! Make sure whenever you add any new field to this object, to update merge() accordingly !!!!!!!!//
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

    insensitive_map<std::string, shared_ptr<Script>> scripts;
    insensitive_map<std::string, shared_ptr<Label>>  labels;
    insensitive_map<std::string, shared_ptr<Var>>    global_vars;
    std::vector<std::shared_ptr<Scope>>              local_scopes;

    IncluderTable ictable;

    uint32_t offset_global_vars = 0;
};

inline auto get_base_var_annotation(const SyntaxTree& var_node) -> optional<shared_ptr<Var>>
{
    if(auto opt = var_node.maybe_annotation<const shared_ptr<Var>&>())
        return *opt;
    else if(auto opt = var_node.maybe_annotation<const ArrayAnnotation&>())
        return opt->base;
    else
        return nullopt;
}
