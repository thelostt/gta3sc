#pragma once
#include "stdinc.h"
#include "annotation.hpp"
#include "parser.hpp"

// TODO fix circular references to shared_ptr<Script> on tree nodes. (e.g. on Label, which has a shared_ptr to Script)

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
struct CompiledScmHeader;
struct Label;

/// Represents a *.sc script file.
struct Script : std::enable_shared_from_this<Script>
{
    static shared_ptr<Script> create(fs::path path_, ScriptType type)
    {
        auto p = std::make_shared<Script>(std::move(path_), type, priv_ctor());
        p->start_label = std::make_shared<Label>(nullptr, p->shared_from_this());
        p->top_label = std::make_shared<Label>(nullptr, p->shared_from_this());
        return p;
    }


    /// Annnotates this script's syntax tree with informations to simplify the compilation step.
    /// For example, annotates whether a identifier is a variable, enum, label, etc.
    ///
    /// \warning This method is not thread-safe because it modifies states! BLA BLA BLA.
    void annotate_tree(const SymTable& symbols, const Commands& commands, ProgramContext& program);

    /// Scans the subdirectory (recursively) named after the name of this script file.
    /// \returns map of (filename, filepath) to all script files found.
    std::map<std::string, fs::path, iless> scan_subdir() const;

    /// Calculates and sets the `offset` field for all the scripts in the `scripts` vector.
    ///
    /// \warning This method is not thread-safe because it modifies states! No compilation step that makes use
    /// of the scripts in the `scripts` vector should be running while this method is executed.
    static void compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts, size_t header_size);

    /// Calculates the model indices (for `models` field)  for all the scripts in the `scripts` vector.
    ///
    /// \returns A list of models that should be put in the SCM header.
    ///
    /// \warning This method is not thread-safe because it modifies states! No compilation step that makes use
    /// of the scripts in the `scripts` vector should be running while this method is executed.
    static auto compute_unknown_models(const std::vector<shared_ptr<Script>>& scripts) -> std::vector<std::string>;

    fs::path                path;
    ScriptType              type;
    shared_ptr<TokenStream> tstream;        // may be nullptr
    shared_ptr<SyntaxTree>  tree;

    shared_ptr<Label>       top_label;      // the label on the very very top of the script
    shared_ptr<Label>       start_label;    // the label to jump into when starting this script.

    /// The offset of this script, in bytes, in the fully compiled SCM.
    /// TODO explain further on which compilation step this value gets to be available.
    optional<uint32_t>      offset;

    /// The full size, in bytes, of the script if already available.
    /// TODO explain further on which compilation step this value gets to be available.
    optional<uint32_t>      size;

    /// If `this->type == ScriptType::Mission`, contains the index of this mission.
    /// This value is made available just before the AST annotation step.
    optional<uint16_t>      mission_id;

    /// List of unknown models referenced by this script.
    /// TODO explain further on which compilation step this value gets to be available.
    std::vector<std::pair<std::string, int32_t>> models;

public:
    optional<int32_t> find_model(const std::string& name) const
    {
        auto it = std::find_if(models.begin(), models.end(), [&](const auto& mpair) {
            return iequal_to()(mpair.first, name);
        });
        if(it != models.end())
            return it->second;
        return nullopt;
    }

    int32_t add_or_find_model(const std::string& name)
    {
        if(auto opt = this->find_model(name))
            return *opt;
        return this->models.emplace(models.end(), name, models.size())->second;
    }

    int32_t find_model_at(uint32_t i) const
    {
        return this->models.at(i).second;
    }

private:
    struct priv_ctor {};

public:
    /// Use create instead.
    explicit Script(fs::path path_, ScriptType type, priv_ctor)
        : type(type)
    {
        this->path = std::move(path_);
        this->tstream.reset(new TokenStream(this->path)); // use new instead of make_shared,
                                                          // we don't want weak_ptr to leave the TokenStream on memory.
        this->tree = SyntaxTree::compile(*this->tstream);
    }

    // TODO a flag to not use tstream (free up memory)?
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

    /// \returns the byte offset (index*4) on which this variable ends in memory (i.e. where the next variable is).
    uint32_t end_offset() { return (index + space_taken()) * 4; }
};

/// Scope information.
struct Scope
{
    std::map<std::string, shared_ptr<Var>, iless> vars;
};

/// Label information.
struct Label
{
    shared_ptr<const Scope>   scope;
    shared_ptr<const Script>  script;

    optional<uint32_t> local_offset;    // relative to `script` base

    Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script)
        : scope(std::move(scope)), script(std::move(script))
    {}

    Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script, uint32_t local_offset)
        : scope(std::move(scope)), script(std::move(script)), local_offset(local_offset)
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
    // TODO keep using iless or use another solution?

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
    // IMPORTANT! Make sure whenever you add any new field to this object, to update merge() accordingly !!!!!!!!//
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

    std::map<std::string, shared_ptr<Script>>       scripts;
    std::map<std::string, shared_ptr<Label>, iless> labels;
    std::map<std::string, shared_ptr<Var>, iless>   global_vars;
    std::vector<std::shared_ptr<Scope>>             local_scopes;

    std::vector<std::string>    extfiles;   /// GOSUB_FILE scripts
    std::vector<std::string>    subscript;  /// LAUNCH_MISSION scripts
    std::vector<std::string>    mission;    /// LOAD_AND_LAUNCH_MISSION scripts

    int32_t count_collectable1 = 0;
    int32_t count_mission_passed = 0;
    int32_t count_progress = 0;
    uint16_t count_set_progress_total = 0;
    uint16_t count_set_total_number_of_missions = 0;
    uint16_t count_set_collectable1_total = 0;


    /// Construts a SymTable from the symbols in `script`.
    static SymTable from_script(Script& script, const Commands& commands, ProgramContext& program)
    {
        SymTable symbols;
        symbols.scan_symbols(script, commands, program);
        return symbols;
    }

    /// Scans all symbols in `script` and adds them to this table.
    ///
    /// \warning This method is not thread-safe because it modifies states! BLA BLA BLA.
    void scan_symbols(Script& script, const Commands& commands, ProgramContext& program);

    /// \warning This method is not thread-safe because it modifies states! BLA BLA BLA.
    void build_script_table(const std::vector<shared_ptr<Script>>& scripts);

    /// \warning
    void check_command_count(ProgramContext& program) const;

    /// Creates a new scope in this table.
    shared_ptr<Scope> add_scope()
    {
        return *local_scopes.emplace(local_scopes.end(), new Scope());
    }

    /// Returns the pointer to the highest global variable (based on index), or nullopt
    /// if there's no global variable in the table.
    optional<shared_ptr<Var>> highest_global_var() const;

    /// Returns the size (in bytes) of the space required to store the global variables in this table.
    size_t size_global_vars() const
    {
        if(auto highest_var = this->highest_global_var())
            return (*highest_var)->end_offset();
        return 0;
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


    /// Adds a new label to the table, returns `nullopt` if it already exists.
    optional<shared_ptr<Label>> add_label(const std::string& name, shared_ptr<const Scope> scope, shared_ptr<const Script> script)
    {
        auto it = this->labels.emplace(name, std::make_shared<Label>(scope, script));
        if(it.second == false)
            return nullopt;
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
    /// Returns false and sends a error to `program` if declaration conflicts with previosly ones.
    bool add_script(ScriptType type, const SyntaxTree& command, ProgramContext& program);

    optional<shared_ptr<Script>> find_script(const std::string& filename) const
    {
        auto it = this->scripts.find(filename);
        if(it != this->scripts.end())
            return it->second;
        return nullopt;
    }

    /// Merges symbol table `t2` into this one.
    /// May output non-fatal errors to `program`.
    void merge(SymTable t2, ProgramContext& program);

    /// Shifts all global vars offsets by `offset`. The unit is not bytes, but indices.
    void apply_offset_to_vars(uint32_t offset_in_index)
    {
        for(auto& var : global_vars)
            var.second->index += offset_in_index;
    }
};

auto read_script(const std::string& filename, const std::map<std::string, fs::path, iless>& subdir,
                 ScriptType type, const Commands& commands, ProgramContext& program) -> optional<shared_ptr<Script>>;


template<typename InputIt> inline
auto read_and_scan_symbols(const std::map<std::string, fs::path, iless>& subdir,
                           InputIt begin, InputIt end, ScriptType type,
                           const Commands& commands, ProgramContext& program) -> std::vector<std::pair<shared_ptr<Script>, SymTable>>
{
    std::vector<std::pair<shared_ptr<Script>, SymTable>> output;

    for(auto it = begin; it != end; ++it)
    {
        if(auto opt_script = read_script(*it, subdir, type, commands, program))
        {
	    shared_ptr<Script> script = std::move(*opt_script);
            SymTable symtable = SymTable::from_script(*script, commands, program);
            output.emplace_back(std::make_pair(std::move(script), std::move(symtable)));
        }
    }

    return output;
}
