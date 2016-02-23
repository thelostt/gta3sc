#include <memory>
#include <map>
#include <string>
#include <vector>
#include "parser.hpp"

#include "cxx17/optional.hpp"

#include "thirdparty/cppformat/format.h"

struct iless
{
    bool operator()(const std::string& left, const std::string& right) const
    {
        return this->operator()(left.c_str(), right.c_str());
    }

    bool operator()(const char* left, const std::string& right) const
    {
        return this->operator()(left, right.c_str());
    }

    bool operator()(const std::string& left, const char* right) const
    {
        return this->operator()(left.c_str(), right);
    }

    bool operator()(const char* left, const char* right) const
    {
        return (_stricmp(left, right) < 0);
    }
};

struct iequal_to
{
    bool operator()(const std::string& left, const std::string& right) const
    {
        return this->operator()(left.c_str(), right.c_str());
    }

    bool operator()(const char* left, const std::string& right) const
    {
        return this->operator()(left, right.c_str());
    }

    bool operator()(const std::string& left, const char* right) const
    {
        return this->operator()(left.c_str(), right);
    }

    bool operator()(const char* left, const char* right) const
    {
        return (_stricmp(left, right) == 0);
    }
};

//////////////

using std::shared_ptr;
using std::weak_ptr;

struct Var;
struct Script;

struct CompilerError : std::runtime_error
{
    template<typename... Args>
    CompilerError(const std::string& msg, Args&&... xargs)
        : std::runtime_error( fmt::format(msg, std::forward<Args>(xargs)...) )
    {}

    CompilerError(const CompilerError&) = default;
    CompilerError(CompilerError&&) = default;
    CompilerError& operator=(const CompilerError&) = default;
    CompilerError& operator=(CompilerError&&) = default;
};

struct Scope
{
    std::map<std::string, shared_ptr<Var>, iless> vars;
};

enum class VarType
{
    Int,
    Float,
    TextLabel,
    TextLabel16,
};

enum class ScriptType
{
    Main,
    MainExtension,
    Subscript,
    Mission,
};


// <global, VarType>
inline std::pair<bool, VarType> token_to_vartype(NodeType token_type)
{
    switch(token_type)
    {
        case NodeType::VAR_INT:
            return std::make_pair(true, VarType::Int);
        case NodeType::VAR_FLOAT:
            return std::make_pair(true, VarType::Float);
        case NodeType::VAR_TEXT_LABEL:
            return std::make_pair(true, VarType::TextLabel);
        case NodeType::VAR_TEXT_LABEL16:
            return std::make_pair(true, VarType::TextLabel16);
        case NodeType::LVAR_INT:
            return std::make_pair(false, VarType::Int);
        case NodeType::LVAR_FLOAT:
            return std::make_pair(false, VarType::Float);
        case NodeType::LVAR_TEXT_LABEL:
            return std::make_pair(false, VarType::TextLabel);
        case NodeType::LVAR_TEXT_LABEL16:
            return std::make_pair(false, VarType::TextLabel16);
        default:
            assert(!"wut");
    }
}

struct Var
{
    const bool                global;   // is global?
    const VarType             type;     // type of var
    const uint32_t            index;    // start index
    const optional<uint32_t>  count;    // num elements of array

    Var(bool global, VarType type, uint32_t index, optional<uint32_t> count)
        : global(global), type(type), index(index), count(count)
    {}

    // space, in indices (4b), that this var takes
    uint32_t space_taken()
    {
        switch(type)
        {
            case VarType::Int:
                return 1 * count.value_or(1);
            case VarType::Float:
                return 1 * count.value_or(1);
            case VarType::TextLabel:
                return 2 * count.value_or(1);
            case VarType::TextLabel16:
                return 4 * count.value_or(1);
            default:
                assert(!"??");
        }
    }
};

struct Label
{
    shared_ptr<const Scope>   scope;
    shared_ptr<const Script>  script;

    Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script)
        : scope(std::move(scope)), script(std::move(script))
    {}
};

struct SymTable
{
    std::map<std::string, shared_ptr<Label>, iless> labels;
    std::map<std::string, shared_ptr<Var>, iless>   global_vars;
    std::vector<std::shared_ptr<Scope>>             local_scopes;

    std::vector<std::string>    extfiles;
    std::vector<std::string>    subscript;
    std::vector<std::string>    mission;

    shared_ptr<Scope> add_scope()
    {
        return *local_scopes.emplace(local_scopes.end());
    }

    bool add_label(std::string name, shared_ptr<const Scope> scope, shared_ptr<const Script> script)
    {
        // TODO put compile error here instead?
        auto it = this->labels.emplace(name, std::make_shared<Label>(scope, script));
        return it.second;
    }

    void add_script(ScriptType type, const SyntaxTree& command)
    {
        auto& vector = (type == ScriptType::MainExtension? this->extfiles :
                        type == ScriptType::Subscript? this->subscript :
                        type == ScriptType::Mission? this->mission :
                        assert(!"A"), this->mission);

        auto& args = command.child(1);

        // ignore validity of parameters for now?
        if(args.child_count() >= 1)
        {
            auto script_name = args.child(0).text();

            auto it = std::find_if(vector.begin(), vector.end(), [&](const auto& a) {
                return iequal_to()(a, script_name);
            });

            if(it == vector.end())
            {
                vector.emplace_back(std::move(script_name));
            }
        }
    }



    void merge(SymTable t2)
    {
        auto& t1 = *this;

        decltype(labels) int_labels;
        decltype(global_vars) int_gvars;

        std::set_intersection( t1.labels.begin(), t1.labels.end(),
                               t2.labels.begin(), t2.labels.end(),
                               std::inserter(int_labels, int_labels.begin()),
                               t1.labels.value_comp() );

        std::set_intersection( t1.global_vars.begin(), t1.global_vars.end(),
                               t2.global_vars.begin(), t2.global_vars.end(),
                               std::inserter(int_gvars, int_gvars.begin()),
                               t1.global_vars.value_comp() );

        if(int_labels.size() > 0)
            throw CompilerError("TODO dup label between script units");
        
        if(int_gvars.size() > 0)
            throw CompilerError("TODO dup global var between script units");

        t1.labels.insert(std::make_move_iterator(t2.labels.begin()),
                         std::make_move_iterator(t2.labels.end()));
        
        t1.global_vars.insert(std::make_move_iterator(t2.global_vars.begin()),
                              std::make_move_iterator(t2.global_vars.end()));

        t1.local_scopes.reserve( t1.local_scopes.size() + t2.local_scopes.size() );
        std::move(t2.local_scopes.begin(), t2.local_scopes.end(), std::back_inserter(t1.local_scopes));

        t1.extfiles.reserve(t1.extfiles.size() + t2.extfiles.size());
        std::move(t2.extfiles.begin(), t2.extfiles.end(), std::back_inserter(t1.extfiles));

        t1.subscript.reserve(t1.subscript.size() + t2.subscript.size());
        std::move(t2.subscript.begin(), t2.subscript.end(), std::back_inserter(t1.subscript));

        t1.mission.reserve(t1.mission.size() + t2.mission.size());
        std::move(t2.mission.begin(), t2.mission.end(), std::back_inserter(t1.mission));
    }
};

struct Script : std::enable_shared_from_this<Script>
{
    fs::path    path;       // ...order of those members...
    SyntaxTree  tree;       // ...is important for construction...

    ScriptType type;

    Script(fs::path path_, ScriptType type)
        : type(type),
          path(std::move(path_)),
          tree( SyntaxTree::compile(TokenStream(this->path)) )
    {
    }

    SymTable scan_symbols() const;

    std::map<std::string, fs::path, iless> scan_subdir() const;
};

template<typename String1, typename String2>
bool is_command(const String1& l, const String2& r)
{
    return iequal_to()(l, r);
}

SymTable Script::scan_symbols() const
{
    std::shared_ptr<Scope> current_scope = nullptr;
    size_t global_index = 0, local_index = 0;

    SymTable table;
    
    std::function<bool(const SyntaxTree&)> walker;

    walker = [&](const SyntaxTree& node)
    {
        switch(node.type())
        {
            case NodeType::Label:
            {
                auto name = node.child(0).text();

                if( !table.add_label(name, current_scope, this->shared_from_this()) )
                    throw CompilerError("Label {} already exists.", name);

                return false;
            }

            case NodeType::Scope:
            {
                if(current_scope)
                    throw CompilerError("Already inside a scope.");

                local_index = 0;
                current_scope = table.add_scope();
                {
                    this->tree.walk(std::ref(walker));
                }
                current_scope = nullptr;

                return false;
            }

            case NodeType::Command:
            {
                auto name = node.child(0).text();

                if(is_command(name, "LOAD_AND_LAUNCH_MISSION"))
                    table.add_script(ScriptType::Mission, node);
                else if(is_command(name, "LAUNCH_MISSION"))
                    table.add_script(ScriptType::Subscript, node);
                else if(is_command(name, "GOSUB_FILE"))
                    table.add_script(ScriptType::MainExtension, node);

                return false;
            }

            case NodeType::VAR_INT: case NodeType::LVAR_INT:
            case NodeType::VAR_FLOAT: case NodeType::LVAR_FLOAT:
            case NodeType::VAR_TEXT_LABEL: case NodeType::LVAR_TEXT_LABEL:
            case NodeType::VAR_TEXT_LABEL16: case NodeType::LVAR_TEXT_LABEL16:
            {
                bool global; VarType vartype;

                std::tie(global, vartype) = token_to_vartype(node.type());

                if(!global && current_scope == nullptr)
                    throw CompilerError("Local var definition outside scope.");

                auto& target = global? table.global_vars : current_scope->vars;
                auto& index  = global? global_index : local_index;

                for(size_t i = 0, max = node.child_count(); i < max; ++i)
                {
                    auto& varnode = node.child(i);

                    auto name = varnode.text();
                    auto count = optional<uint32_t>(nullopt);

                    if(varnode.child_count())
                    {
                        auto array_counter = std::stol(varnode.child(0).text(), nullptr, 0);

                        if(array_counter <= 0)
                            throw CompilerError("Negative or zero array counter {}.", name);

                        count.emplace(array_counter);
                    }

                    auto it = target.emplace(name, std::make_shared<Var>(global, vartype, index, count));

                    if(it.second == false)
                        throw CompilerError("Variable {} already exists.", name);

                    index += it.first->second->space_taken();
                }

                return false;
            }

            default:
                return true;
        }
    };

    this->tree.walk(std::ref(walker));
    return table;
}

std::map<std::string, fs::path, iless> Script::scan_subdir() const
{
    auto output = std::map<std::string, fs::path, iless>();
    auto subdir = this->path.parent_path() / this->path.stem();

    for(auto& entry : fs::recursive_directory_iterator(subdir))
    {
        auto filename = entry.path().filename().generic_u8string();
        output.emplace(std::move(filename), entry.path());
    }

    return output;
}


template<typename InputIt>
auto read_and_scan_symbols(const std::map<std::string, fs::path, iless>& subdir,
                           InputIt begin, InputIt end, ScriptType type)           -> std::vector<std::pair<shared_ptr<Script>, SymTable>>
{
    std::vector<std::pair<shared_ptr<Script>, SymTable>> output;

    for(auto it = begin; it != end; ++it)
    {
        auto path_it = subdir.find(*it);
        if(path_it == subdir.end())
            throw CompilerError("File '{}' does not exist in '{}' subdirectory.", *it, "main");

        auto script = std::make_shared<Script>(path_it->second, type);
        auto symtable = script->scan_symbols();
        output.emplace_back( std::make_pair(std::move(script), std::move(symtable)) );
    }

    return output;
}

int main()
{
    _wchdir(L"../..");

    auto main = std::make_shared<Script>("main.sc", ScriptType::Main);
    auto symbols = main->scan_symbols();

    auto subdir = main->scan_subdir();

    auto ext_scripts = read_and_scan_symbols(subdir, symbols.extfiles.begin(), symbols.extfiles.end(), ScriptType::MainExtension);
    auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript);
    auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission);

    for(auto& x : ext_scripts) symbols.merge(std::move(x.second));
    for(auto& x : sub_scripts) symbols.merge(std::move(x.second));
    for(auto& x : mission_scripts) symbols.merge(std::move(x.second));





    //getchar();
}

