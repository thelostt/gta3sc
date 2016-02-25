#include <memory>
#include <map>
#include <string>
#include <vector>
#include "parser.hpp"

#include <numeric>

#include "cxx17/variant.hpp"
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
struct CodeGenerator;

struct CompilerError : std::runtime_error
{
    template<typename... Args>
    CompilerError(const std::string& msg, Args&&... args)
        : std::runtime_error( fmt::format(msg, std::forward<Args>(args)...) )
    {}

    CompilerError(const CompilerError&) = default;
    CompilerError(CompilerError&&) = default;
    CompilerError& operator=(const CompilerError&) = default;
    CompilerError& operator=(CompilerError&&) = default;
};

struct BadAlternator : CompilerError
{
    template<typename... Args>
    BadAlternator(const std::string& msg, Args&&... args)
        : CompilerError(msg, std::forward<Args>(args)...)
    {}

    BadAlternator(const BadAlternator&) = default;
    BadAlternator(BadAlternator&&) = default;
    BadAlternator& operator=(const BadAlternator&) = default;
    BadAlternator& operator=(BadAlternator&&) = default;
};

struct Scope
{
    std::map<std::string, shared_ptr<Var>> vars;
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
                return 0;
        }
    }
};

struct Label
{
    shared_ptr<const Scope>   scope;
    shared_ptr<const Script>  script;

    optional<uint32_t> local_offset;    // relative to script base
    optional<uint32_t> global_offset;

    Label(shared_ptr<const Scope> scope, shared_ptr<const Script> script)
        : scope(std::move(scope)), script(std::move(script))
    {}
};

struct SymTable
{
    std::map<std::string, shared_ptr<Label>> labels;
    std::map<std::string, shared_ptr<Var>>   global_vars;
    std::vector<std::shared_ptr<Scope>>      local_scopes;

    std::vector<std::string>    extfiles;
    std::vector<std::string>    subscript;
    std::vector<std::string>    mission;

    shared_ptr<Scope> add_scope()
    {
        return *local_scopes.emplace(local_scopes.end());
    }

    void add_label(const std::string& name, shared_ptr<const Scope> scope, shared_ptr<const Script> script)
    {
        // TODO put compile error here instead?
        auto it = this->labels.emplace(name, std::make_shared<Label>(scope, script));
        if(it.second == false)
            throw CompilerError("Label {} already exists.", name);
    }

    optional<shared_ptr<Label>> find_label(const std::string& name) const
    {
        auto it = this->labels.find(name);
        if(it != this->labels.end())
            return it->second;
        return nullopt;
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

    void compute_label_offsets_globally();
};

struct Script : std::enable_shared_from_this<Script>
{
    fs::path                path;       // ...order of those members...
    shared_ptr<SyntaxTree>  tree;       // ...is important for construction...

    ScriptType type;

    // global offset
    optional<uint32_t>  offset;

    optional<uint32_t>  size;


    Script(fs::path path_, ScriptType type)
        : type(type),
          path(std::move(path_)),
          tree( SyntaxTree::compile(TokenStream(this->path)) )
    {
    }

    SymTable scan_symbols() const;

    std::map<std::string, fs::path, iless> scan_subdir() const;

    // this will modify all Script's in the vector, careful on threading situations
    static void compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts)
    {
        size_t offset = 0; // TODO header, pass as argument too
        for(auto& script_ptr : scripts)
        {
            script_ptr->offset.emplace(offset);
            offset += script_ptr->size.value();
        }
    }
};

void SymTable::compute_label_offsets_globally()
{
    for(auto it = this->labels.begin(); it != this->labels.end(); ++it)
    {
        auto& label_ptr = it->second;
        label_ptr->global_offset = label_ptr->script->offset.value() + label_ptr->local_offset.value();
    }
}

SymTable Script::scan_symbols() const
{
    std::function<bool(const SyntaxTree&)> walker;

    // states for the scan
    std::shared_ptr<Scope> current_scope = nullptr;
    size_t global_index = 0, local_index = 0;
    std::string next_scoped_label;

    // the scan output
    SymTable table;
    
    // the scanner
    walker = [&](const SyntaxTree& node)
    {
        switch(node.type())
        {
            case NodeType::Label:
            {
                shared_ptr<SyntaxTree> parent = node.parent();   // should always be available for me

                auto next = parent->find(node);
                if(next != parent->end() && ++next != parent->end()
                    && (*next)->type() == NodeType::Scope)
                {
                    next_scoped_label = node.child(0).text();
                }
                else
                {
                    auto& name = node.child(0).text();
                    table.add_label(name, current_scope, this->shared_from_this());
                }
                return false;
            }

            case NodeType::Scope:
            {
                if(current_scope)
                    throw CompilerError("Already inside a scope.");

                local_index = 0;
                current_scope = table.add_scope();
                {
                    if(!next_scoped_label.empty())
                    {
                        table.add_label(next_scoped_label, current_scope, this->shared_from_this());
                        next_scoped_label.clear();
                    }

                    node.walk(std::ref(walker));
                }
                current_scope = nullptr;

                return false;
            }

            case NodeType::Command:
            {
                auto name = node.child(0).text();

                if(name == "LOAD_AND_LAUNCH_MISSION")
                    table.add_script(ScriptType::Mission, node);
                else if(name == "LAUNCH_MISSION")
                    table.add_script(ScriptType::Subscript, node);
                else if(name == "GOSUB_FILE")
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

    this->tree->walk(std::ref(walker));
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


enum class ArgType : uint8_t
{
    Any,
    Label,
    Buffer128,
    Integer,
    Float,
    TextLabel,
};

bool argtype_matches(ArgType type1, ArgType type2)
{
    return type1 == type2 || type1 == ArgType::Any || type2 == ArgType::Any;
}

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

const Command& Commands::match(const SyntaxTree& command_node) const
{
    auto num_args    = command_node.child_count() - 1;
    auto alter_range = commands.equal_range( command_node.child(0).text() );

    for(auto alter_kv = alter_range.first; alter_kv != alter_range.second; ++alter_kv)
    {
        size_t arg_readen     = 0;

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

Commands get_test_commands()
{
    return Commands
    {{
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
    }};
}

struct half
{
    float value;
};

struct EOAL {}; // End of Argument List

struct CompiledVar
{
    shared_ptr<Var>                            var;
    optional<variant<size_t, shared_ptr<Var>>> index;

    size_t compiled_size() const
    {
        // TODO
        return 0;
    }
};

struct CompiledString
{
    enum class Type : uint8_t
    {
        Buffer8_3VC, // no datatype
        Buffer8,
        Buffer16,
        BufferVar,
        Buffer128,
    };

    Type        type;
    std::string storage;

    size_t compiled_size() const
    {
        switch(this->type)
        {
            case CompiledString::Type::Buffer8_3VC:
                return 8;
            case CompiledString::Type::Buffer8:
                return 1 + 8;
            case CompiledString::Type::Buffer16:
                return 1 + 16;
            case CompiledString::Type::BufferVar:
                return 1 + 1 + this->storage.size();
            case CompiledString::Type::Buffer128:
                return 128;
            default:
                assert(!"");
                return 0;
        }
    }
};

using ArgVariant = variant<EOAL, int8_t, int16_t, int32_t, half, float, shared_ptr<Label>, CompiledVar, CompiledString>;

template<typename T>
size_t compiled_size(const T& x)
{
    return x.compiled_size();
}

template<typename T>
void generate_code(const T& x)
{
    return x.generate_code();
}

size_t compiled_size(const EOAL&)
{
    return 1;
}

size_t compiled_size(const int8_t&)
{
    return 1 + sizeof(int8_t);
}

size_t compiled_size(const int16_t&)
{
    return 1 + sizeof(int16_t);
}

size_t compiled_size(const int32_t&)
{
    return 1 + sizeof(int32_t);
}

size_t compiled_size(const half&)
{
    return 1 + sizeof(int16_t);
}

size_t compiled_size(const float&)
{
    return 1 + sizeof(float);
}

size_t compiled_size(const shared_ptr<Label>&)
{
    return 1 + sizeof(int32_t);
}

size_t compiled_size(const ArgVariant& varg)
{
    return visit_one(varg, [](const auto& arg) { return ::compiled_size(arg); });
}

struct CompiledCommand
{
    uint16_t                 id;
    std::vector<ArgVariant> args;

    size_t compiled_size() const
    {
        size_t size = sizeof(uint16_t);
        for(auto& a : args) size += ::compiled_size(a);
        return size;
    }
};

// Label definitions have a compiled size of 0, and thus need to be in another struct
// otherwise it'd return sizeof(int32_t).
struct CompiledLabelDef
{
    shared_ptr<Label> label;

    size_t compiled_size() const
    {
        return 0;
    }
};

struct CompiledData
{
    variant<CompiledLabelDef, CompiledCommand> data;

    CompiledData() = delete;

    CompiledData(CompiledCommand x)
        : data(std::move(x))
    {}

    CompiledData(shared_ptr<Label> x)
        : data(CompiledLabelDef { std::move(x) })
    {}

    size_t compiled_size() const
    {
        return visit_one(this->data, [](const auto& data) { return ::compiled_size(data); });
    }

    void generate_code(CodeGenerator& bytecode) const;
};



struct CompilerContext
{
    const Commands&             commands;
    const shared_ptr<Script>    script;
    const SymTable&             symbols;

    std::vector<CompiledData>   compiled;

    CompilerContext(shared_ptr<Script> script, const SymTable& symbols, const Commands& commands)
        : script(std::move(script)), symbols(symbols), commands(commands)
    {}

    void compile()
    {
        return compile_statements(*script->tree);
    }

    // finds local_offsets for labels and returns size of this script
    uint32_t compute_labels() const
    {
        uint32_t offset = 0;
        for(auto& op : this->compiled)
        {
            if(is<CompiledLabelDef>(op.data))
            {
                get<CompiledLabelDef>(op.data).label->local_offset = offset;
            }
            else
            {
                offset += op.compiled_size();
            }
        }
        return offset;
    }

private:
    void compile_statements(const SyntaxTree& base)
    {
        for(auto it = base.begin(); it != base.end(); ++it)
        {
            switch((*it)->type())
            {
                case NodeType::Command:
                    compile_command(*it->get());
                    break;
                case NodeType::Label:
                    compile_label(*it->get());
                    break;
                case NodeType::Scope:
                    break;
                case NodeType::IF:
                    break;
                case NodeType::WHILE:
                    break;
                case NodeType::REPEAT:
                    break;
                case NodeType::SWITCH:
                    break;
                case NodeType::BREAK:
                    break;
                case NodeType::CONTINUE:
                    break;
                case NodeType::VAR_INT:
                    break;
                case NodeType::LVAR_INT:
                    break;
                case NodeType::VAR_FLOAT:
                    break;
                case NodeType::LVAR_FLOAT:
                    break;
                case NodeType::VAR_TEXT_LABEL:
                    break;
                case NodeType::LVAR_TEXT_LABEL:
                    break;
                case NodeType::VAR_TEXT_LABEL16:
                    break;
                case NodeType::LVAR_TEXT_LABEL16:
                    break;
                default:
                    assert(!"");
            }
        }
    }

    void compile_label(const SyntaxTree& label_node)
    {
        // XXX PERF this can (maybe) be faster if we cache the shared_ptr<Label> as a userdata in its SyntaxTree node
        // during the scan_symbols compilation step. The cons of that is that we'll turn SyntaxTree into a mutable thing.
        // So, do this only in last case if we need more performance.

        shared_ptr<Label> label = *this->symbols.find_label(label_node.child(0).text());
        this->compiled.emplace_back(std::move(label));
    }

    void compile_command(const SyntaxTree& command_node)
    {
        auto& command_name = command_node.child(0).text();
        auto  num_args     = command_node.child_count() - 1;

        const Command& command       = this->commands.match(command_node);
        std::vector<ArgVariant> args = get_args(command, command_node);

        this->compiled.emplace_back(CompiledCommand { command.id, std::move(args) });
    }

    std::vector<ArgVariant> get_args(const Command& command, const SyntaxTree& command_node)
    {
        std::vector<ArgVariant> args;
        args.reserve(command_node.child_count() - 1);

        for(auto ita = command_node.begin() + 1; ita != command_node.end(); ++ita)
        {
            const SyntaxTree& arg_node = **ita;

            switch(arg_node.type())
            {
                case NodeType::Integer:
                {
                    args.emplace_back( get_int(arg_node.text()) );
                    break;
                }
                case NodeType::Float:
                {
                    args.emplace_back( get_float(arg_node.text()) );
                    break;
                }
                case NodeType::Array:
                {
                    // TODO
                    break;
                }
                case NodeType::Identifier:
                {
                    if(auto opt_label = this->symbols.find_label(arg_node.text()))
                    {
                        args.emplace_back(*opt_label);
                    }

                    // TODO
                    break;
                }
                case NodeType::ShortString:
                {
                    // TODO
                    break;
                }
                case NodeType::LongString:
                {
                    // TODO
                    break;
                }
            }
        }

        return args;
    }

    ArgVariant get_int(const std::string& s)
    {
        int32_t i = std::stoi(s, nullptr, 0);

        if(i >= std::numeric_limits<int8_t>::min() && i <= std::numeric_limits<int8_t>::max())
            return int8_t(i);
        else if(i >= std::numeric_limits<int16_t>::min() && i <= std::numeric_limits<int16_t>::max())
            return int16_t(i);
        else
            return int32_t(i);
    }

    ArgVariant get_float(const std::string& s)
    {
        // TODO GTA III
        return std::stof(s);
    }
};

struct CodeGenerator
{
    const shared_ptr<Script>    script;
    std::vector<CompiledData>   compiled;
    const SymTable&             symbols;
    
    std::unique_ptr<uint8_t[]>  bytecode; // size == script->size
    size_t                      offset;

    CodeGenerator(shared_ptr<Script> script_, std::vector<CompiledData> compiled, const SymTable& symbols) :
        script(std::move(script_)), compiled(std::move(compiled)), symbols(symbols)
    {
        this->offset = 0;
        this->bytecode.reset(new uint8_t[this->script->size.value()]);
    }

    CodeGenerator(CompilerContext context) : // consumes the context (faster)
        CodeGenerator(std::move(context.script), std::move(context.compiled), context.symbols)
    {}

    /*
    CodeGenerator(const CompilerContext& context) : // does not consume the context (slower)
        CodeGenerator(context.script, context.compiled, context.symbols)
    {}
    */

    void generate()
    {
        assert(this->offset == 0); // Expects
        for(auto& op : this->compiled)
        {
            op.generate_code(*this);
        }
    }

    void emplace_u8(int8_t value)
    {
        assert(this->offset + 1 <= this->script->size); // debug check
        bytecode[this->offset++] = reinterpret_cast<uint8_t&>(value);
    }

    void emplace_u16(int16_t value)
    {
        // TODO maybe optimize, write a entire i16 at a time? is that portable?
        //assert(this->offset + 2 <= this->script->size);
        emplace_u8((value & 0x00FF) >> 0);
        emplace_u8((value & 0xFF00) >> 8);
    }

    void emplace_u32(int32_t value)
    {
        // TODO maybe optimize, write a entire i32 at a time? is that portable?
        //assert(this->offset + 4 <= this->script->size);
        emplace_u8((value & 0x000000FF) >> 0);
        emplace_u8((value & 0x0000FF00) >> 8);
        emplace_u8((value & 0x00FF0000) >> 16);
        emplace_u8((value & 0xFF000000) >> 24);
    }

    void emplace_i8(uint8_t value)
    {
        return emplace_u8(reinterpret_cast<uint8_t&>(value));
    }

    void emplace_i16(uint16_t value)
    {
        return emplace_u16(reinterpret_cast<uint16_t&>(value));
    }

    void emplace_i32(uint32_t value)
    {
        return emplace_u32(reinterpret_cast<uint32_t&>(value));
    }
};


void generate_code(const EOAL&, CodeGenerator& codegen)
{
    codegen.emplace_u8(0);
}

void generate_code(const int8_t& value, CodeGenerator& codegen)
{
    codegen.emplace_u8(4);
    codegen.emplace_i8(value);
}

void generate_code(const int16_t& value, CodeGenerator& codegen)
{
    codegen.emplace_u8(5);
    codegen.emplace_i16(value);
}

void generate_code(const int32_t& value, CodeGenerator& codegen)
{
    codegen.emplace_u8(1);
    codegen.emplace_i32(value);
}

void generate_code(const half& value, CodeGenerator& codegen)
{
    // TODO half
    codegen.emplace_u8(5);
    codegen.emplace_u16(0);
}

void generate_code(const float& value, CodeGenerator& codegen)
{
    static_assert(std::numeric_limits<float>::is_iec559
        && sizeof(float) == sizeof(uint32_t), "IEEE 754 floating point expected.");

    codegen.emplace_u8(5);
    codegen.emplace_u32(reinterpret_cast<const uint32_t&>(value));
}

void generate_code(const shared_ptr<Label>& label_ptr, CodeGenerator& codegen)
{
    codegen.emplace_u8(1);
    codegen.emplace_i32(label_ptr->global_offset.value());
}

void generate_code(const ArgVariant& varg, CodeGenerator& codegen)
{
    return visit_one(varg, [&](const auto& arg) { return ::generate_code(arg, codegen); });
}

void generate_code(const CompiledLabelDef&, CodeGenerator&)
{
    // label definitions do not have a physical representation
}

void generate_code(const CompiledString&, CodeGenerator&)
{
    // TODO
}

void generate_code(const CompiledVar&, CodeGenerator&)
{
    // TODO
}

void generate_code(const CompiledCommand& ccmd, CodeGenerator& codegen)
{
    codegen.emplace_u16(ccmd.id);
    for(auto& arg : ccmd.args) ::generate_code(arg, codegen);
}

void CompiledData::generate_code(CodeGenerator& codegen) const
{
    return visit_one(this->data, [&](const auto& data) { return ::generate_code(data, codegen); });
}




int main()
{
    _wchdir(L"../..");

    std::vector<shared_ptr<Script>> scripts;

    auto commands = get_test_commands();

    auto main = std::make_shared<Script>("test.sc", ScriptType::Main);
    auto symbols = main->scan_symbols();

    scripts.emplace_back(main);

    auto subdir = main->scan_subdir();

    auto ext_scripts = read_and_scan_symbols(subdir, symbols.extfiles.begin(), symbols.extfiles.end(), ScriptType::MainExtension);
    auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript);
    auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission);

    for(auto& x : ext_scripts)
    {
        symbols.merge(std::move(x.second));
        scripts.emplace_back(x.first); // maybe move
    }

    for(auto& x : sub_scripts)
    {
        symbols.merge(std::move(x.second));
        scripts.emplace_back(x.first); // maybe move
    }

    for(auto& x : mission_scripts)
    {
        symbols.merge(std::move(x.second));
        scripts.emplace_back(x.first); // maybe move
    }

    CompilerContext cc(main, symbols, commands);
    cc.compile();
    main->size = cc.compute_labels();

    Script::compute_script_offsets(scripts);
    symbols.compute_label_offsets_globally();
    
    CodeGenerator cgen(std::move(cc));
    cgen.generate();

    FILE* f = fopen("output.cs", "wb");
    fwrite(cgen.bytecode.get(), 1, cgen.script->size.value(), f);
    fclose(f);

    //getchar();
}

