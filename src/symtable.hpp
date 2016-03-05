#pragma once
#include "stdinc.h"
#include "parser.hpp"
#include "error.hpp"

struct Command;

/// Declared type of a variable.
enum class VarType
{
    Int,
    Float,
    TextLabel,
    TextLabel16,
};

/// Type of a script file (*.sc).
enum class ScriptType
{
    Main,           /// The script file with the main declarations.
    MainExtension,  /// Imported using GOSUB_FILE
    Subscript,      /// Imported using LAUNCH_MISSION
    Mission,        /// Imported using LOAD_AND_LAUNCH_MISSION
};

/// Converts VAR_INT, LVAR_FLOAT, etc tokens into the VarType enum.
/// \returns pair(is_global, vartype).
std::pair<bool, VarType> token_to_vartype(NodeType token_type);

struct SymTable;
struct Commands;

/// Represents a *.sc script file.
struct Script : std::enable_shared_from_this<Script>
{
    Script(fs::path path_, ScriptType type)
        : type(type)
    {
        this->path = std::move(path_);
        this->tree = SyntaxTree::compile(TokenStream(this->path));
    }

    /// Annnotates this script's syntax tree with informations to simplify the compilation step.
    /// For example, annotates whether a identifier is a variable, enum, label, etc.
    ///
    /// \warning This method is not thread-safe because it modifies states! BLA BLA BLA.
    void annotate_tree(const SymTable& symbols, const Commands& commands);

    /// Scans the subdirectory (recursively) named after the name of this script file.
    /// \returns map of (filename, filepath) to all script files found.
    std::map<std::string, fs::path, iless> scan_subdir() const;

    /// Calculates and sets the `offset` field for all the scripts in the `scripts` vector.
    ///
    /// \warning This method is not thread-safe because it modifies states! No compilation step that makes use
    /// of the scripts in the `scripts` vector should be running while this method is executed.
    static void compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts);


    fs::path                path;
    ScriptType              type;
    shared_ptr<SyntaxTree>  tree;

    /// The offset of this script, in bytes, in the fully compiled SCM.
    /// TODO explain further on which compilation step this value gets to be available.
    optional<uint32_t>      offset;

    /// The full size, in bytes, of the script if already available.
    /// TODO explain further on which compilation step this value gets to be available.
    optional<uint32_t>      size;
};

/// Information about a previosly declared variable.
struct Var
{
    const bool                global;
    const VarType             type;
    uint32_t                  index; /// (not offset, index indeed)
    const optional<uint32_t>  count; /// If an array, the number of elements of it.

    Var(bool global, VarType type, uint32_t index, optional<uint32_t> count)
        : global(global), type(type), index(index), count(count)
    {}

    /// \returns the space, in words (i.e. bytes/4), that this variable takes in memory.
    uint32_t space_taken();

    /// \returns the byte offset (index*4) on which this variable is in memory.
    uint32_t offset() { return index * 4; }
};

/// Scope information.
struct Scope
{
    std::map<std::string, shared_ptr<Var>> vars;
};

/// Label information.
struct Label
{
    shared_ptr<const Scope>   scope;
    shared_ptr<const Script>  script;

    optional<uint32_t> local_offset;    // relative to `script` base
    optional<uint32_t> global_offset;

    Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script)
        : scope(std::move(scope)), script(std::move(script))
    {}

    /// Returns the global offset for this label.
    uint32_t offset()
    {
        return script->offset.value() + this->local_offset.value();
    }
};

/// Stores important symbols defined throught scripts (labels, vars, scopes, subscripts).
struct SymTable
{
    // TODO check conflicts of label names and global names!?! (on merge too)

    // IMPORTANT! Make sure whenever you add any new field, to update merge() accordingly !!!!!!!!!!

    std::map<std::string, shared_ptr<Label>> labels;
    std::map<std::string, shared_ptr<Var>>   global_vars;
    std::vector<std::shared_ptr<Scope>>      local_scopes;

    std::vector<std::string>    extfiles;   /// GOSUB_FILE scripts
    std::vector<std::string>    subscript;  /// LAUNCH_MISSION scripts
    std::vector<std::string>    mission;    /// LOAD_AND_LAUNCH_MISSION scripts

    /// Construts a SymTable from the symbols in `script`.
    static SymTable from_script(Script& script)
    {
        SymTable symbols;
        symbols.scan_symbols(script);
        return symbols;
    }

    /// Scans all symbols in `script` and adds them to this table.
    ///
    /// \warning This method is not thread-safe because it modifies states! BLA BLA BLA.
    void scan_symbols(Script& script);

    /// Creates a new scope in this table.
    shared_ptr<Scope> add_scope()
    {
        return *local_scopes.emplace(local_scopes.end(), new Scope());
    }

    /// Finds global var `name` or local var `name` in `current_scope`. `current_scope` may be nullptr,
    /// otherwise it must be a scope owned by this symbol table.
    optional<shared_ptr<Var>> find_var(const std::string& name, const shared_ptr<Scope>& current_scope) const
    {
        auto itg = global_vars.find(name);
        if(itg != global_vars.end())
        {
            return itg->second;
        }

        if(current_scope)
        {
            assert(std::any_of(local_scopes.begin(), local_scopes.end(), [&](const auto& scope_ptr) {
                return current_scope == scope_ptr;
            }));

            auto itl = current_scope->vars.find(name);
            if(itl != current_scope->vars.end())
            {
                return itl->second;
            }
        }

        return nullopt;
    }

    /// \throws CompilerError if label already exists.
    shared_ptr<Label> add_label(const std::string& name, shared_ptr<const Scope> scope, shared_ptr<const Script> script)
    {
        auto it = this->labels.emplace(name, std::make_shared<Label>(scope, script));
        if(it.second == false)
            throw CompilerError("Label {} already exists.", name);
        return it.first->second;
    }

    /// Finds label `name` in this table.
    optional<shared_ptr<Label>> find_label(const std::string& name) const
    {
        auto it = this->labels.find(name);
        if(it != this->labels.end())
            return it->second;
        return nullopt;
    }

    /// Informs about a external script file with `type` referenced by `command`
    /// \throws CompilerError if declaration conflicts with previosly ones.
    void add_script(ScriptType type, const SyntaxTree& command);

    /// Merges symbol table `t2` into this one.
    void merge(SymTable t2);

    /// Shifts all global vars offsets by `offset`. The unit is not bytes, but indices.
    void apply_offset_to_vars(uint32_t offset_in_index)
    {
        for(auto& var : global_vars)
            var.second->index += offset_in_index;
    }
};

template<typename InputIt> inline
auto read_and_scan_symbols(const std::map<std::string, fs::path, iless>& subdir, InputIt begin, InputIt end, ScriptType type) 
-> std::vector<std::pair<shared_ptr<Script>, SymTable>>
{
    std::vector<std::pair<shared_ptr<Script>, SymTable>> output;

    for(auto it = begin; it != end; ++it)
    {
        auto path_it = subdir.find(*it);
        if(path_it == subdir.end())
            throw CompilerError("File '{}' does not exist in '{}' subdirectory.", *it, "main");

        auto script = std::make_shared<Script>(path_it->second, type);
        auto symtable = SymTable::from_script(*script);
        output.emplace_back(std::make_pair(std::move(script), std::move(symtable)));
    }

    return output;
}

//////

struct RepeatAnnotation
{
    const Command& set_var_to_zero;
    const Command& add_var_with_one;
    const Command& is_var_geq_times;
    // numbers compatible with set_var_to_times and add_var_with_int
    shared_ptr<SyntaxTree> number_zero;
    shared_ptr<SyntaxTree> number_one;
};
