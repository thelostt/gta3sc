#include <stdinc.h>
#include "commands.hpp"

/// Type of a script file (*.sc).
enum class ScriptType
{
    Main,           //< The script file with the main declarations.
    MainExtension,  //< Imported using GOSUB_FILE
    Subscript,      //< Imported using LAUNCH_MISSION
    Mission,        //< Imported using LOAD_AND_LAUNCH_MISSION
    StreamedScript, //< Imported using REGISTER_STREAMED_SCRIPT
    CustomScript,   //< Main custom script
    CustomMission,  //< Main custom mission
    Required,       //< Imported using REQUIRE
};

/// Represents a *.sc script file.
class Script : public std::enable_shared_from_this<Script>
{
public:
    using SubDir = insensitive_map<std::string, fs::path>;

public:
    /// \returns `nullptr` on failure and populates `program` with errors, otherwise the script object.
    static shared_ptr<Script> create(fs::path path, ScriptType type, ProgramContext& program);

    /// Creates a `Script` which has `filename` in the subdirectory object `subdir` of this main script.
    /// \returns `nullptr` on failure and populates `program` with errors, otherwise the script object.
    shared_ptr<Script> from_subdir(const string_view& filename, const SubDir& subdir,
                                   ScriptType type, ProgramContext& program) const;

    /// Scans the subdirectory (recursively) named after the name of this script file.
    /// \returns map of (filename, filepath) to all script files found.
    auto scan_subdir() const -> SubDir;

    /// Annnotates this script syntax tree with informations to simplify the compilation step.
    /// For example, annotates whether a identifier is a variable, enum, label, and such.
    /// \warning this method is not exactly thread-safe.
    void annotate_tree(const SymTable& symbols, ProgramContext& program);

    /// Computes the output types of every call scope in this script.
    /// \warning this method is not exactly thread-safe.
    void compute_scope_outputs(const SymTable& symbols, ProgramContext& program);

    /// For mission scripts, fixes the variable indices in call scopes.
    /// \warning this method is not exactly thread-safe.
    void fix_call_scope_variables(ProgramContext& program);

    /// Calculates and sets the `offset` field for all the scripts in the `scripts` vector.
    /// \warning this method is not thread-safe.
    static void compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts, const MultiFileHeaderList&);

    /// Calculates the model indices (for `models` field)  for all the scripts in the `scripts` vector.
    /// \returns A list of models that should be put in the SCM header.
    /// \warning this method is not thread-safe.
    static auto compute_used_objects(const std::vector<shared_ptr<Script>>& scripts)->std::vector<std::string>;

    /// Handles things such as procedure calling and entity information, on which the order of finding is important.
    /// \warning this method is not thread-safe.
    static void handle_special_commands(const std::vector<shared_ptr<Script>>&, SymTable&, ProgramContext&);

    /// Adds a required script to this script.
    /// \warning this method is not exactly thread-safe.
    void add_children(shared_ptr<Script> script);

    /// \returns whether this script uses local offsets naturally.
    bool uses_local_offsets() const;

    /// \returns whether this script is on the same block as `other`.
    bool on_the_same_space_as(const Script& other) const;

    /// \returns whether this is `type`, or a required script from one.
    bool is_child_of(ScriptType type) const;

    /// \returns whether this is a mission/custom mission, or a required script from one.
    bool is_child_of_mission() const;

    /// \returns whether this is a custom script/mission, or a required script from one.
    bool is_child_of_custom() const;

    /// \returns the root script of this required script, or itself if not a required script.
    shared_ptr<const Script> root_script() const;

    /// \returns whether this is not a required script.
    bool is_root_script() const;

    /// \returns whether this was the input script.
    bool is_main_script() const;

    /// \returns the code offset from the root script to this script.
    size_t distance_from_root() const;

    /// Finds the highest local variable indices used by general scopes (.first) and call scopes (.second).
    /// \note also searches in children scripts.
    /// \note if the last local index used was e.g. 0+, then this returns 1+. If no local was used, returns 0.
    std::pair<uint32_t, uint32_t> find_maximum_locals() const;

    /// \returns the size of the headers in this script.
    uint32_t header_size() const
    {
        return (this->code_offset.value() - this->base.value());
    }

    /// \returns the size of this script including its headers and children scripts.
    uint32_t full_size() const
    {
        size_t size = this->code_size.value() + this->header_size();
        for(auto& sc : children_scripts) size += sc.lock()->full_size();
        return size;
    }

    /// Finds whether the unknown model `name` was used in this script, and its usage index.
    optional<int32_t> find_model(const string_view& name) const
    {
        auto it = std::find_if(models.begin(), models.end(), [&](const auto& mpair) {
            return iequal_to()(mpair.first, name);
        });
        if(it != models.end())
            return it->second;
        return nullopt;
    }

    /// Does the same as `find_model`, except it adds the model if none was found.
    int32_t add_or_find_model(const string_view& name)
    {
        if(auto opt = this->find_model(name))
            return *opt;
        return this->models.emplace(models.end(), name.to_string(), models.size())->second;
    }

    /// Finds the unknown model index at position `i`.
    int32_t find_model_at(uint32_t i) const
    {
        return this->models.at(i).second;
    }

public:
    const fs::path          path;
    const ScriptType        type;
    const shared_ptr<TokenStream> tstream;
    shared_ptr<SyntaxTree>  tree;

    shared_ptr<Label>       top_label;      //< Label on the very top of the script, before any command.
    shared_ptr<Label>       start_label;    //< Label to jump into when starting this script.

    /// The offset of this script, in bytes, in the fully compiled SCM.
    /// This value is made available before/during the code generation step.
    /// \note This is the same as `offset` but including the script headers.
    optional<uint32_t>      base;

    /// The offset of this **script code**, in bytes, in the fully compiled SCM.
    /// This value is made available before/during the code generation step.
    /// \note This is the offset to the script code, not including the headers, use `base` to include headers.
    optional<uint32_t>      code_offset;

    /// The full size, in bytes, of the script if already available.
    /// This value is made available before/during the code generation step.
    /// \note This is the size of the script **without** headers.
    optional<uint32_t>      code_size;

    /// If `this->type == ScriptType::Mission`, contains the index of this mission.
    /// This value is made available just before the AST annotation step.
    optional<uint16_t>      mission_id;

    /// If `this->type == ScriptType::StreamedScript`, contains the index of this streamed script.
    /// This value is made available just before the AST annotation step.
    optional<uint16_t>      streamed_id;

    /// All the scopes within this script.
    std::vector<shared_ptr<Scope>> scopes;

    // Required scripts.
    std::vector<weak_ptr<const Script>> children_scripts;   //< Required scripts.
    weak_ptr<const Script>              parent_script;      //< Parent of required script.

private:
    /// List of used models referenced by this script.
    /// This value is made available after the AST annotation step.
    std::vector<std::pair<std::string, int32_t>> models;

private:
    // Use Script::create or Script::from_subdir instead.
    explicit Script(ProgramContext& program, ScriptType type, fs::path path_,
        shared_ptr<TokenStream> tstream, shared_ptr<SyntaxTree> tree)
        : type(type), path(std::move(path_)), tstream(std::move(tstream)), tree(std::move(tree))
    {
    }
};

/// Information about a previously declared variable.
struct Var
{
    weak_ptr<const SyntaxTree>where; //< Declaration node or expired() if none.
    const bool                global;
    const VarType             type;
    EntityType                entity;///< The entity type of this variable. \note only avaiabile after Script::verify_special_commands(...). 
    uint32_t                  index; ///< Variable index (not offset). \note this value is not well-defined until the ir-generation step.
    const optional<uint32_t>  count; ///< If an array, the number of elements of it.

    explicit Var(weak_ptr<const SyntaxTree> where, bool global, VarType type, uint32_t index, optional<uint32_t> count)
        : where(std::move(where)), entity(0), global(global), type(type), index(index), count(count)
    {}

    explicit Var(bool global, VarType type, uint32_t index, optional<uint32_t> count)
        : Var(weak_ptr<const SyntaxTree>(), global, type, index, std::move(count))
    {}

    /// \returns the space, in words (i.e. bytes/4), that this variable takes in memory.
    uint32_t space_taken();

    /// \returns the space, in words (i.e. bytes/4), that a variable of type `type` and `count` would take in memory.
    static uint32_t space_taken(VarType type, size_t count = 1);

    /// \returns the byte offset (index*4) on which this variable is in memory.
    uint32_t offset() {
        return index * 4;
    }

    /// \returns the byte offset (index*4) on which this variable ends in memory (i.e. where the next variable is).
    uint32_t end_offset() {
        return (index + space_taken()) * 4;
    }

    /// \returns whether this is a TEXT_LABEL or TEXT_LABEL16 variable.
    bool is_text_var() {
        return type == VarType::TextLabel || type == VarType::TextLabel16;
    }
};

/// Scope information.
class Scope
{
public:
    using OutputType = VarType; // Int, Float, TextLabel=String; TextLabel16 is unused.
    using OutputVector = std::vector<std::pair<OutputType, weak_ptr<Var>>>;

public:
    insensitive_map<std::string, shared_ptr<Var>>   vars;       //< The variables in this scope.
    
    explicit Scope(weak_ptr<SyntaxTree> tree) :
        tree(std::move(tree))
    {}

    /// Whether this is a call scope.
    bool is_call_scope() const { return this->outputs != nullopt; }

    /// Returns the variable at the specified local index.
    shared_ptr<Var> var_at(size_t index) const;

protected:
    weak_ptr<SyntaxTree>    tree;       //< The scope node (of type NodeType::Scope)
    optional<OutputVector>  outputs;    //< If this is a call scope, the outputs of the scope.

    static optional<OutputType> output_type_from_node(const SyntaxTree& node);

    friend class Script;
};

/// Label information.
struct Label
{
    shared_ptr<const Scope>   scope;        //< The scope of this label.
    weak_ptr<const Script>    script;       //< The script of this label (weak to avoid circular reference)
    weak_ptr<const SyntaxTree>where;        //< Where this label was declared (may be expired() for unknown)
    optional<uint32_t>        code_position;//< Relative to `script->code_offset`.

    explicit Label(weak_ptr<const SyntaxTree> where, shared_ptr<const Scope> scope, shared_ptr<const Script> script)
        : where(std::move(where)), scope(std::move(scope)), script(std::move(script))
    {}

    explicit Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script)
        : Label(weak_ptr<const SyntaxTree>(), std::move(scope), std::move(script))
    {}

    explicit Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script, uint32_t code_position)
        : scope(std::move(scope)), script(std::move(script)), code_position(code_position)
    {}

    /// \returns whether a branch from `other_script` into this label is possible.
    bool may_branch_from(const Script& other_script, ProgramContext& program) const;

    ///  \returns the global offset for this label.
    uint32_t offset() const
    {
        return script.lock()->code_offset.value() + this->code_position.value();
    }

    /// \returns the local offset for this label relative to `script->root_script()->base`.
    uint32_t distance_from_base() const
    {
        auto script = this->script.lock();
        return script->root_script()->header_size() + script->distance_from_root() + this->code_position.value();
    }
};

inline const char* to_string(ScriptType type)
{
    switch(type)
    {
        case ScriptType::Main:           return "main";
        case ScriptType::MainExtension:  return "extension";
        case ScriptType::Subscript:      return "subscript";
        case ScriptType::Mission:        return "mission";
        case ScriptType::StreamedScript: return "streamed";
        case ScriptType::CustomMission:  return "custom mission";
        case ScriptType::CustomScript:   return "custom";
        case ScriptType::Required:       return "required";
        default:                         Unreachable();
    }
}
