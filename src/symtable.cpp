#include "symtable.hpp"
#include "commands.hpp"

uint32_t Var::space_taken()
{
    switch(this->type)
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
            Unreachable();
    }
}

std::pair<bool, VarType> token_to_vartype(NodeType token_type)
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
            Unreachable();
    }
}

void Script::compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts)
{
    // TODO header, pass as argument too
    size_t offset = 0;
    for(auto& script_ptr : scripts)
    {
        Expects(script_ptr->offset == nullopt);
        script_ptr->offset.emplace(offset);
        offset += script_ptr->size.value();
    }
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

void SymTable::add_script(ScriptType type, const SyntaxTree& command)
{
    auto& args = command.child(1);
    
    if(args.child_count() < 1)
    {
        throw CompilerError("TODO few args");
    }
    else
    {
        auto script_name = args.child(0).text();

        auto searcher = [&](const std::string& other) {
            return iequal_to()(other, script_name);
        };

        bool found_extfile   = std::any_of(this->extfiles.begin(), this->extfiles.end(), searcher);
        bool found_subscript = std::any_of(this->subscript.begin(), this->subscript.end(), searcher);
        bool found_mission   = std::any_of(this->mission.begin(), this->mission.end(), searcher);

        if(found_extfile && type != ScriptType::MainExtension)
            throw CompilerError("TODO incompatible, previous declaration blabla");
        else if(found_subscript && type != ScriptType::Subscript)
            throw CompilerError("TODO incompatible, previous declaration blabla");
        else if(found_mission && type != ScriptType::Mission)
            throw CompilerError("TODO incompatible, previous declaration blabla");
        else 
        {
            // still not added to its list?
            if(!found_extfile && !found_subscript && !found_mission)
            {
                auto& vector = (type == ScriptType::MainExtension? this->extfiles :
                    type == ScriptType::Subscript? this->subscript :
                    type == ScriptType::Mission? this->mission : Unreachable());

                vector.emplace_back(std::move(script_name));
            }
        }
    }
}

void SymTable::merge(SymTable t2)
{
    // TODO improve readability of this

    auto& t1 = *this;

    decltype(labels) int_labels;
    decltype(global_vars) int_gvars;

    // finds items that are common in both
    std::set_intersection(t1.labels.begin(), t1.labels.end(),
        t2.labels.begin(), t2.labels.end(),
        std::inserter(int_labels, int_labels.begin()),
        t1.labels.value_comp());

    // finds items that are common in both
    std::set_intersection(t1.global_vars.begin(), t1.global_vars.end(),
        t2.global_vars.begin(), t2.global_vars.end(),
        std::inserter(int_gvars, int_gvars.begin()),
        t1.global_vars.value_comp());

    // TODO check for conflicting extfiles, subscripts and mission

    if(int_labels.size() > 0)
        throw CompilerError("TODO dup label between script units");

    if(int_gvars.size() > 0)
        throw CompilerError("TODO dup global var between script units");

    shared_ptr<Var> highest_var;
    uint32_t begin_t2_vars = 0;
    for(const auto& var : t1.global_vars)
    {
        if(highest_var == nullptr || var.second->index > highest_var->index)
        {
            highest_var = var.second;
            begin_t2_vars = highest_var->index + highest_var->space_taken();
        }
    }
    t2.apply_offset_to_vars(begin_t2_vars);

    t1.labels.insert(std::make_move_iterator(t2.labels.begin()),
        std::make_move_iterator(t2.labels.end()));

    t1.global_vars.insert(std::make_move_iterator(t2.global_vars.begin()),
        std::make_move_iterator(t2.global_vars.end()));

    t1.local_scopes.reserve(t1.local_scopes.size() + t2.local_scopes.size());
    std::move(t2.local_scopes.begin(), t2.local_scopes.end(), std::back_inserter(t1.local_scopes));

    t1.extfiles.reserve(t1.extfiles.size() + t2.extfiles.size());
    std::move(t2.extfiles.begin(), t2.extfiles.end(), std::back_inserter(t1.extfiles));

    t1.subscript.reserve(t1.subscript.size() + t2.subscript.size());
    std::move(t2.subscript.begin(), t2.subscript.end(), std::back_inserter(t1.subscript));

    t1.mission.reserve(t1.mission.size() + t2.mission.size());
    std::move(t2.mission.begin(), t2.mission.end(), std::back_inserter(t1.mission));
}

void SymTable::scan_symbols(Script& script)
{
    std::function<bool(SyntaxTree&)> walker;

    // states for the scan
    std::shared_ptr<Scope> current_scope = nullptr;
    size_t global_index = 0, local_index = 0;
    shared_ptr<SyntaxTree> next_scoped_label;

    // the scan output
    SymTable& table = *this;

    // the scanner
    walker = [&](SyntaxTree& node)
    {
        switch(node.type())
        {
            case NodeType::Label:
            {
                shared_ptr<SyntaxTree> parent = node.parent();   // should always be available for this rule

                auto next = parent->find(node);
                if(next != parent->end() && ++next != parent->end()
                    && (*next)->type() == NodeType::Scope)
                {
                    // we'll add this label later since we need to put it into a scope (the rule following this one)
                    next_scoped_label = node.shared_from_this();
                }
                else
                {
                    auto& label_name = node.child(0).text();
                    auto label_ptr = table.add_label(label_name, current_scope, script.shared_from_this());
                    node.set_annotation(std::move(label_ptr));
                }
                return false;
            }

            case NodeType::Scope:
            {
                if(current_scope)
                    throw CompilerError("Already inside a scope.");

                auto guard = make_scope_guard([&] {
                    current_scope = nullptr;
                });

                local_index = 0;
                current_scope = table.add_scope();
                {
                    if(next_scoped_label)
                    {
                        auto& label_name = next_scoped_label->child(0).text();
                        auto label_ptr = table.add_label(label_name, current_scope, script.shared_from_this());
                        next_scoped_label->set_annotation(std::move(label_ptr));
                        next_scoped_label = nullptr;
                    }

                    node.walk(std::ref(walker));

                    node.set_annotation(std::move(current_scope));
                }
                // guard sets current_scope to nullptr

                return false;
            }

            case NodeType::Command:
            {
                auto name = node.child(0).text();

                // TODO use `const Commands&` to identify these?
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
                auto& index = global? global_index : local_index;

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
                    auto var_ptr = it.first->second;

                    if(it.second == false)
                        throw CompilerError("Variable {} already exists.", name);

                    index += var_ptr->space_taken();

                    varnode.set_annotation(std::move(var_ptr));
                }

                return false;
            }

            default:
                return true;
        }
    };

    script.tree->walk(std::ref(walker));
}

void Script::annotate_tree(const SymTable& symbols, const Commands& commands)
{
    std::function<bool(SyntaxTree&)> walker;

    std::shared_ptr<Scope> current_scope = nullptr;

    walker = [&](SyntaxTree& node)
    {
        switch(node.type())
        {
            case NodeType::Label:
                // already annotated in SymTable::scan_symbols
                return false;

            case NodeType::Scope:
            {
                // already annotated in SymTable::scan_symbols, but let's inform about current scope
                
                auto guard = make_scope_guard([&] {
                    current_scope = nullptr;
                });

                Expects(current_scope == nullptr);

                current_scope = node.annotation<shared_ptr<Scope>>();
                node.walk(std::ref(walker));
                // guard sets current_scope to nullptr

                return false;
            }

            case NodeType::VAR_INT: case NodeType::LVAR_INT:
            case NodeType::VAR_FLOAT: case NodeType::LVAR_FLOAT:
            case NodeType::VAR_TEXT_LABEL: case NodeType::LVAR_TEXT_LABEL:
            case NodeType::VAR_TEXT_LABEL16: case NodeType::LVAR_TEXT_LABEL16:
                // already annotated in SymTable::scan_symbols
                return false;

            case NodeType::Command:
            {
                try
                {
                    const Command& command = commands.match(node, symbols, current_scope);
                    commands.annotate(node, command, symbols, current_scope);
                }
                catch(const BadAlternator&)
                {
                    // ignore this one for now, let compiler.cpp/hpp handle this FOR NOW
                }
                return false;
            }

            case NodeType::Equal:
            {
                // TODO CHECK IF IN A CONDITION
                try
                {
                    auto alternators = commands.set();
                    SyntaxTree& left = node.child(0);
                    SyntaxTree& right = node.child(1);
                    
                    const Command& command = commands.match_args(symbols, current_scope, alternators, left, right);
                    commands.annotate_args(symbols, current_scope, command, left, right);

                    node.set_annotation(std::cref(command));
                }
                catch(const BadAlternator&)
                {
                    // ignore this one for now, let compiler.cpp/hpp handle this FOR NOW
                }
                return false;
            }

            default:
                return true;
        }
    };

    this->tree->walk(std::ref(walker));
}


