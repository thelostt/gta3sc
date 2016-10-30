#include "stdinc.h"
#include "symtable.hpp"
#include "commands.hpp"
#include "program.hpp"
#include "error.hpp"

// TODO check if vars, labels, etc aren't already constants and etc

// TODO android compiler registers START_CUTSCENE stuff before the global variable space

uint32_t Var::space_taken(VarType type, size_t count)
{
    switch(type)
    {
        case VarType::Int:
            return 1 * count;
        case VarType::Float:
            return 1 * count;
        case VarType::TextLabel:
            return 2 * count;
        case VarType::TextLabel16:
            return 4 * count;
        default:
            Unreachable();
    }
}

uint32_t Var::space_taken()
{
    return Var::space_taken(this->type, this->count.value_or(1));
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

bool Label::may_branch_from(const Script& other_script, ProgramContext& program) const
{
    if(this->script->type == ScriptType::Mission
    || this->script->type == ScriptType::StreamedScript)
    {
        if(&other_script != this->script.get())
            return false;
    }
    return true;
}

void Script::compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts, size_t header_size)
{
    size_t offset = header_size;

    for(auto& script_ptr : scripts)
    {
        Expects(script_ptr->offset == nullopt);
        script_ptr->offset.emplace(offset);
        offset += script_ptr->size.value();
    }
}

auto Script::compute_unknown_models(const std::vector<shared_ptr<Script>>& scripts) -> std::vector<std::string>
{
    std::vector<std::string> models;

    for(auto& script : scripts)
    {
        for(auto& umodel : script->models)
        {
            auto it = std::find_if(models.begin(), models.end(), [&](const std::string& m) {
                return iequal_to()(m, umodel.first);
            });

            if(it == models.end())
            {
                if(models.capacity() == 0)
                    models.reserve(200);

                models.emplace_back(umodel.first);
                it = models.begin() + (models.size() - 1);
            }

            umodel.second = -(1 + std::distance(models.begin(), it));
        }
    }

    return models;
}

void Script::process_entity_type(const SyntaxTree& var_node, EntityType arg_type, bool is_output, ProgramContext& program)
{
    if(!program.opt.entity_tracking)
        return;

    if(arg_type != 0)
    {
        auto& var = var_node.annotation<const shared_ptr<Var>&>();
        auto& varinfo = this->add_or_find_varinfo(var);

        if(is_output)
        {
            if(varinfo.entity_type != 0 && varinfo.entity_type != arg_type)
            {
                if(varinfo.entity_assigned)
                {
                    program.error(var_node,
                        "XXX variable has been already used to create entity typed  '{}'",
                        program.commands.find_entity_name(varinfo.entity_type).value()
                    );
                }
                else
                {
                    program.error(var_node,
                        "XXX variable has been previosly used as a entity typed  '{}'",
                        program.commands.find_entity_name(varinfo.entity_type).value()
                    );
                }
            }

            varinfo.entity_type = arg_type;
            varinfo.entity_assigned = true;
        }
        else
        {
            if(varinfo.entity_type == 0)
            {
                // Do not error, maybe it was assigned in another script.
                // We'll check this in verify_entity_types.
                varinfo.entity_used_before_assign = true;
            }
            else if(varinfo.entity_type != arg_type)
            {
                program.error(var_node,
                    "XXX variable expected to have entity typed '{}', but variable has entity typed '{}' ",
                    program.commands.find_entity_name(arg_type).value(),
                    program.commands.find_entity_name(varinfo.entity_type).value()
                );
            }

            varinfo.entity_type = arg_type;
        }
    }
}

void Script::assign_entity_type(const SyntaxTree& lhs, const SyntaxTree& rhs, ProgramContext& program)
{
    if(!program.opt.entity_tracking)
        return;

    auto opt_lhs_var = lhs.maybe_annotation<const shared_ptr<Var>&>();
    auto opt_rhs_var = rhs.maybe_annotation<const shared_ptr<Var>&>();

    if(opt_lhs_var && opt_rhs_var)
    {
        this->assign_entity_type(*opt_lhs_var, *opt_rhs_var, rhs, program);
    }
}

void Script::assign_entity_type(const shared_ptr<Var>& dst_var, const shared_ptr<Var>& src_var,
                                const SyntaxTree& error_helper, ProgramContext& program)
{
    if(!program.opt.entity_tracking)
        return;

    EntityType lhs_type = 0;
    EntityType rhs_type = 0;

    {
        auto opt_lhsinfo = this->find_varinfo(dst_var);
        auto opt_rhsinfo = this->find_varinfo(src_var);
        if(opt_lhsinfo) lhs_type = opt_lhsinfo->entity_type;
        if(opt_rhsinfo) rhs_type = opt_rhsinfo->entity_type;
    }

    // NONE = NONE
    if(lhs_type == 0 && rhs_type == 0)
        return;

    // ENTITY = NONE or ENTITY = ENTITY (with different types)
    if(lhs_type != 0 && lhs_type != rhs_type)
    {
        program.error(error_helper,
            "XXX destination variable has entity type '{}', but trying to assign variable with entity type '{}'",
            program.commands.find_entity_name(lhs_type).value(),
            program.commands.find_entity_name(rhs_type).value()
        );
    }

    // NONE = ENTITY or ENTITY = ENTITY
    if(rhs_type != 0)
    {
        auto& lhsinfo = this->add_or_find_varinfo(dst_var);
        auto& rhsinfo = this->find_varinfo(src_var).value();
        lhsinfo.entity_assigned = true;
        lhsinfo.entity_type     = rhsinfo.entity_type;
    }
}

void Script::verify_entity_types(const std::vector<shared_ptr<Script>>& scripts,
                                 const SymTable& symtable, ProgramContext& program)
{
    if(!program.opt.entity_tracking)
        return;

    /// Keep track of entities assigned in previous scripts.
    std::map<shared_ptr<Var>, EntityType> assigned_vars;

    for(auto s1 = scripts.begin(); s1 != scripts.end(); ++s1)
    {
        for(auto& vinfo_pair : (*s1)->varinfo)
        {
            auto& var    = vinfo_pair.first;
            auto& vinfo1 = vinfo_pair.second;

            // If at this script the variable has been used as a entity before being assigned a entity,
            // ensure any previous script assigned a entity to the variable.
            if(vinfo1.entity_used_before_assign)
            {
                if(assigned_vars.find(var) == assigned_vars.end())
                {
                    program.error(**s1,
                        "XXX variable '{}' expected to have entity typed '{}', but had '{}' during its usage", // had 'NONE'
                        symtable.find_var_name(var).value(), 
                        program.commands.find_entity_name(vinfo1.entity_type).value(),
                        program.commands.find_entity_name(0).value()
                    );
                }
            }

            // Check mismatch entity with previous scripts.
            auto ait = assigned_vars.find(var);
            if(ait != assigned_vars.end())
            {
                if(ait->second != vinfo1.entity_type)
                {
                    program.error(**s1,
                        "XXX Variable '{}' has different entity types in two or more scripts. First seen as '{}', now as '{}'.",
                        symtable.find_var_name(var).value(),
                        program.commands.find_entity_name(ait->second).value(),
                        program.commands.find_entity_name(vinfo1.entity_type).value()
                    );
                }
            }
            else
            {
                // If this script assigned a entity to the variable, let's keep that info.
                if(vinfo1.entity_assigned)
                {
                    assigned_vars.emplace(var, vinfo1.entity_type);
                }
            }
        }
    }
}

void Script::send_input_vars(const SyntaxTree& target_label_node,
                             SyntaxTree::const_iterator arg_begin, SyntaxTree::const_iterator arg_end,
                             ProgramContext& program)
{
    // TODO handle TEXT_LABEL and TEXT_LABEL16 properly for CALL commands?

    auto check_var_matches = [&](const SyntaxTree& arg_node, const shared_ptr<Var>& lvar) -> bool
    {
        if(auto opt_arg_var = arg_node.maybe_annotation<const shared_ptr<Var>&>())
        {
            this->assign_entity_type(lvar, *opt_arg_var, arg_node, program);

            if((*opt_arg_var)->type != lvar->type)
            {
                program.error(arg_node, "XXX type mismatch in target label");
                return false;
            }
        }
        else if(auto opt_arg_var = arg_node.maybe_annotation<const ArrayAnnotation&>())
        {
            auto& arg_var = opt_arg_var->base;

            this->assign_entity_type(lvar, arg_var, arg_node, program);

            if(arg_var->type != lvar->type)
            {
                program.error(arg_node, "XXX type mismatch in target label");
                return false;
            }
        }
        else
        {
            program.error(arg_node, "XXX type mismatch in target label");
            return false;
        }
        return true;
    };

    if(auto opt_target_label = target_label_node.maybe_annotation<const shared_ptr<Label>&>())
    {
        if(auto& target_scope = (*opt_target_label)->scope)
        {
            size_t target_var_index = 0;
            for(auto arg = arg_begin; arg != arg_end; ++arg)
            {
                auto lvar = target_scope->var_at(target_var_index++);

                if(!lvar)
                {
                    program.error(**arg, "XXX not enough vars in target label");
                    break;
                }

                switch(lvar->type)
                {
                    case VarType::Int:
                        if((**arg).maybe_annotation<const int32_t&>())
                            break;
                        check_var_matches(**arg, lvar);
                        break;
                    case VarType::Float:
                        if((**arg).maybe_annotation<const float&>())
                            break;
                        check_var_matches(**arg, lvar);
                        break;
                    case VarType::TextLabel:
                    case VarType::TextLabel16:
                        program.error(**arg, "XXX local vars in target label type mismatch (LVAR_TEXT_LABEL not allowed)");
                        break;
                    default:
                        Unreachable();
                }
            }
        }
        else
        {
            program.error(target_label_node, "XXX Expected scope in target label. Add a '{{' before the target label.");
            // TODO ^ instruction should be different between III/VC (before or after)
        }
    }
}

std::map<std::string, fs::path, iless> Script::scan_subdir() const
{
    auto output = std::map<std::string, fs::path, iless>();
    auto subdir = this->path.parent_path() / this->path.stem();

    if(fs::exists(subdir) && fs::is_directory(subdir))
    {
        for(auto& entry : fs::recursive_directory_iterator(subdir))
        {
            auto filename = entry.path().filename().generic_u8string();
            output.emplace(std::move(filename), entry.path());
        }
    }

    return output;
}

void SymTable::build_script_table(const std::vector<shared_ptr<Script>>& scripts)
{
    // TODO check or not to check if script is in our `extfiles`, `missions` etc?
    for(auto& script : scripts)
    {
        auto name = script->path.filename().u8string();
        std::transform(name.begin(), name.end(), name.begin(), ::toupper); // TODO UTF-8 able
        this->scripts.emplace(std::move(name), script);
    }
}

void SymTable::check_command_count(ProgramContext& program) const
{
    if(this->count_set_progress_total > 1)
        program.error(nocontext, "XXX SET_PROGRESS_TOTAL occurs multiple times between script units, it should appear only once.");
    if(this->count_set_collectable1_total > 1)
        program.error(nocontext, "XXX SET_COLLECTABLE1_TOTAL occurs multiple times between script units, it should appear only once.");
    if(this->count_set_total_number_of_missions > 1)
        program.error(nocontext, "XXX SET_TOTAL_NUMBER_OF_MISSIONS occurs multiple times between script units, it should appear only once.");
}

void Script::verify_script_names(const std::vector<shared_ptr<Script>>& scripts, ProgramContext& program)
{
    if(!program.opt.script_name_check)
        return;

    std::map<std::string, shared_ptr<Script>, iless> names;
    for(auto sc1 = scripts.begin(); sc1 != scripts.end(); ++sc1)
    {
        for(auto& name : (**sc1).script_names)
        {
            auto inpair = names.emplace(name, *sc1);
            if(!inpair.second)
            {
                program.error(**sc1, "XXX script name '{}' previosly used in {}",
                              inpair.first->first, inpair.first->second->path.generic_u8string());
            }
        }
    }
}

bool SymTable::add_script(ScriptType type, const SyntaxTree& command, ProgramContext& program)
{
    if(command.child_count() <= 1)
    {
        program.error(command, "XXX few args");
        return false;
    }
    else
    {
        // TODO R* compiler checks if parameter ends with .sc

        auto script_name = (command.child(0).text() == "GOSUB_FILE"?
                                command.child(2).text() :
                                command.child(1).text());

        auto searcher = [&](const std::string& other) {
            return iequal_to()(other, script_name);
        };

        bool found_extfile   = std::any_of(this->extfiles.begin(), this->extfiles.end(), searcher);
        bool found_subscript = std::any_of(this->subscript.begin(), this->subscript.end(), searcher);
        bool found_mission   = std::any_of(this->mission.begin(), this->mission.end(), searcher);
        bool found_streamed  = std::any_of(this->streamed.begin(), this->streamed.end(), searcher);

        if(found_extfile && type != ScriptType::MainExtension)
        {
            program.error(command, "XXX incompatible, previous declaration blabla");
            return false;
        }
        else if(found_subscript && type != ScriptType::Subscript)
        {
            program.error(command, "XXX incompatible, previous declaration blabla");
            return false;
        }
        else if(found_mission && type != ScriptType::Mission)
        {
            program.error(command, "XXX incompatible, previous declaration blabla");
            return false;
        }
        else if(found_streamed && type != ScriptType::StreamedScript)
        {
            program.error(command, "XXX incompatible, previous declaration blabla");
            return false;
        }
        else 
        {
            // still not added to its list?
            if(!found_extfile && !found_subscript && !found_mission && !found_streamed)
            {
                auto refvector = (type == ScriptType::MainExtension? std::ref(this->extfiles) :
                    type == ScriptType::Subscript? std::ref(this->subscript) :
                    type == ScriptType::Mission? std::ref(this->mission) :
                    type == ScriptType::StreamedScript? std::ref(this->streamed) : Unreachable());

                refvector.get().emplace_back(std::move(script_name));
            }
            return true;
        }
    }
}

optional<shared_ptr<Var>> SymTable::highest_global_var() const
{
    auto fn_comp = [](const auto& apair, const auto& bpair) {
        const shared_ptr<Var>& a = apair.second;
        const shared_ptr<Var>& b = bpair.second;
        if(a->index < b->index)
            return true;
        else if(a->index != b->index)
            return false;
        else
            return a->space_taken() < b->space_taken();
    };

    auto it = std::max_element(this->global_vars.begin(), this->global_vars.end(), fn_comp);
    if(it == this->global_vars.end())
        return nullopt;
    return it->second;
}

void SymTable::merge(SymTable t2, ProgramContext& program)
{
    // TODO improve readability of this

    auto& t1 = *this;

    decltype(labels) int_labels;
    decltype(global_vars) int_gvars;
    decltype(scripts) int_scripts;

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

    // TODO check for conflicting extfiles, subscripts, mission and streamed

    if(int_labels.size() > 0)
    {
        for(auto& kv : int_labels)
        {
            auto script1_name = t1.labels.find(kv.first)->second->script->path.generic_u8string();
            auto script2_name = t2.labels.find(kv.first)->second->script->path.generic_u8string();
            program.error(nocontext, "XXX duplicate label '{}' between script units '{}' and '{}'.", kv.first, script1_name, script2_name);
        }
    }

    if(int_gvars.size() > 0)
    {
        for(auto& kv : int_gvars)
        {
            // XXX maybe store in Var the Script where it was declared?
            program.error(nocontext, "XXX duplicate global var '{}' between script units.", kv.first);
        }
    }


    //
    // All error conditions checked, perform actual merge
    //

    uint32_t begin_t2_vars = t1.size_global_vars() / 4;
    t2.apply_offset_to_vars(begin_t2_vars);

    t1.scripts.insert(std::make_move_iterator(t2.scripts.begin()),
        std::make_move_iterator(t2.scripts.end()));

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

    t1.streamed.reserve(t1.streamed.size() + t2.streamed.size());
    std::move(t2.streamed.begin(), t2.streamed.end(), std::back_inserter(t1.streamed));

    t1.count_progress += t2.count_progress;
    t1.count_collectable1 += t2.count_collectable1;
    t1.count_mission_passed += t2.count_mission_passed;

    t1.count_set_progress_total += t2.count_set_progress_total;
    t1.count_set_collectable1_total += t2.count_set_collectable1_total;
    t1.count_set_total_number_of_missions += t2.count_set_total_number_of_missions;
}

void SymTable::scan_symbols(Script& script, ProgramContext& program)
{
    std::function<bool(SyntaxTree&)> walker;

    // states for the scan
    std::shared_ptr<Scope> current_scope = nullptr;
    size_t global_index = 0, local_index = 0;
    shared_ptr<SyntaxTree> next_scoped_label;

    // the scan output
    SymTable& table = *this;

    auto get_progress_value = [&](const SyntaxTree& node)
    {
        if(node.child_count() == 2)
        {
            try
            {
                return std::stoi(node.child(1).text().to_string());
            }
            catch(const std::logic_error&)
            {
                // just emit a warning since R* compiler accepts this construct.
                program.warning(node, "XXX progress value is not a constant");
                return 0;
            }
        }
        else
        {
            program.error(node, "XXX bad num args");
            return 0;
        }
    };

    // the scanner
    walker = [&](SyntaxTree& node)
    {
        switch(node.type())
        {
            case NodeType::Label:
            {
                shared_ptr<SyntaxTree> parent = node.parent();   // should always be available for this rule

                auto next = std::find(parent->begin(), parent->end(), node.shared_from_this());
                Expects(next->get() == &node);
                next = std::next(next);

                if(next != parent->end() && (*next)->type() == NodeType::Scope)
                {
                    // we'll add this label later since we need to put it into a scope (the rule following this one)
                    next_scoped_label = node.shared_from_this();
                }
                else
                {
                    auto label_name = node.text();
                    
                    auto opt_label_ptr = table.add_label(label_name.to_string(), current_scope, script.shared_from_this());
                    if(!opt_label_ptr)
                    {
                        program.error(node, "XXX Label {} already exists", label_name);
                        opt_label_ptr = table.find_label(label_name).value();
                    }

                    Expects(opt_label_ptr != nullopt);
                    node.set_annotation(std::move(*opt_label_ptr)); 
                }
                return false;
            }

            case NodeType::Scope:
            {
                auto guard = make_scope_guard([&, old_current_scope = current_scope] {
                    current_scope = old_current_scope;
                });

                if(current_scope)
                {
                    program.error(node, "XXX Already inside a scope.");
                    // continue using current scope instead of entering a new one
                }
                else
                {
                    local_index = 0;
                    current_scope = table.add_scope();
                    current_scope->vars.emplace("TIMERA", std::make_shared<Var>(false, VarType::Int, program.opt.timer_index + 0, nullopt));
                    current_scope->vars.emplace("TIMERB", std::make_shared<Var>(false, VarType::Int, program.opt.timer_index + 1, nullopt));
                }

                if(next_scoped_label)
                {
                    auto label_name = next_scoped_label->text();
                    
                    if(program.opt.pedantic && program.opt.scope_then_label)
                    {
                        program.error(node, "XXX since VC the label should be inside the scope [-pedantic]");
                    }

                    auto opt_label_ptr = table.add_label(label_name.to_string(), current_scope, script.shared_from_this());
                    if(!opt_label_ptr)
                    {
                        program.error(node, "XXX Label {} already exists", label_name);
                        opt_label_ptr = table.find_label(label_name).value();
                    }

                    Expects(opt_label_ptr != nullopt);
                    next_scoped_label->set_annotation(std::move(*opt_label_ptr));
                    next_scoped_label = nullptr;
                }

                node.child(0).depth_first(std::ref(walker));

                node.set_annotation(std::move(current_scope));

                // guard sets current_scope to previous value (probably nullptr)

                return false;
            }

            case NodeType::Command:
            {
                auto command_name = node.child(0).text();

                uint16_t count_unique_command = 0;

                // TODO use `const Commands&` to identify these?
		// TODO case sensitivity
                if(command_name == "LOAD_AND_LAUNCH_MISSION")
                    table.add_script(ScriptType::Mission, node, program);
                else if(command_name == "LAUNCH_MISSION")
                    table.add_script(ScriptType::Subscript, node, program);
                else if(command_name == "GOSUB_FILE")
                    table.add_script(ScriptType::MainExtension, node, program);
                else if(command_name == "REGISTER_STREAMED_SCRIPT")
                    table.add_script(ScriptType::StreamedScript, node, program);
                else if(command_name == "CREATE_COLLECTABLE1")
                    ++table.count_collectable1;
                else if(command_name == "REGISTER_MISSION_PASSED" || command_name == "REGISTER_ODDJOB_MISSION_PASSED")
                    ++table.count_mission_passed;
                else if(command_name == "PLAYER_MADE_PROGRESS")
                    table.count_progress += get_progress_value(node);
                else if(command_name == "SET_COLLECTABLE1_TOTAL")
                    count_unique_command = ++table.count_set_collectable1_total;
                else if(command_name == "SET_TOTAL_NUMBER_OF_MISSIONS")
                    count_unique_command = ++table.count_set_total_number_of_missions;
                else if(command_name == "SET_PROGRESS_TOTAL")
                    count_unique_command = ++table.count_set_progress_total;

                if(count_unique_command > 1)
                    program.error(node, "XXX {} more than once", command_name);

                return false;
            }

            case NodeType::VAR_INT: case NodeType::LVAR_INT:
            case NodeType::VAR_FLOAT: case NodeType::LVAR_FLOAT:
            case NodeType::VAR_TEXT_LABEL: case NodeType::LVAR_TEXT_LABEL:
            case NodeType::VAR_TEXT_LABEL16: case NodeType::LVAR_TEXT_LABEL16:
            {
                // TODO arrays have a [256] index limit?

                bool global; VarType vartype;

                std::tie(global, vartype) = token_to_vartype(node.type());

                if(!global && current_scope == nullptr)
                {
                    program.error(node, "XXX Local var definition outside scope.");
                    return false;
                }

                if(!program.opt.text_label_vars
                    && (vartype == VarType::TextLabel || vartype == VarType::TextLabel16))
                {
                    program.error(node, "XXX var text label not allowed");
                }

                auto& target = global? table.global_vars : current_scope->vars;
                auto& index = global? global_index : local_index;
                size_t max_index;
                
                if(global)
                    max_index = (65536 / 4);
                else if(script.type == ScriptType::Mission)
                    max_index = program.opt.mission_var_limit.value_or(program.opt.local_var_limit);
                else
                    max_index = program.opt.local_var_limit;

                for(size_t i = 0, max = node.child_count(); i < max; ++i)
                {
                    auto& varnode = node.child(i);

                    auto name  = string_view();
                    auto count = optional<uint32_t>(nullopt);

                    if(auto opt_token = Miss2Identifier::match(varnode.text()))
                    {
                        auto& token = *opt_token;
                        name = token.identifier;

                        if(token.index != nullopt)
                        {
                            if(is<string_view>(*token.index))
                            {
                                // TODO allow enum?
                                program.error(varnode, "XXX non-constant index value in array declaration");
                                count = 1; // fallback
                            }
                            else
                            {
                                count = get<size_t>(*token.index);
                            }
                        }
                    }
                    else
                    {
                        program.error(varnode, opt_token.error().c_str());
                        continue;
                    }

                    if(!program.opt.farrays && count)
                    {
                        program.error(varnode, "XXX arrays not supported [-farrays]");
                    }

                    auto it = target.emplace(name, std::make_shared<Var>(global, vartype, index, count));
                    auto var_ptr = it.first->second;

                    if(it.second == false)
                    {
                        program.error(varnode, "XXX Variable {} already exists.", name);
                        varnode.set_annotation(std::move(var_ptr));
                    }
                    else
                    {
                        index += var_ptr->space_taken();
                        varnode.set_annotation(std::move(var_ptr));
                    }

                    if(index > max_index)
                        program.error(varnode, "XXX max {} vars limit ('{}')", (global? "global" : "local"), max_index);
                }

                return false;
            }

            default:
                return true;
        }
    };

    script.tree->depth_first(std::ref(walker));
}

void Script::annotate_tree(const SymTable& symbols, ProgramContext& program)
{
    const Commands& commands = program.commands;

    std::function<bool(SyntaxTree&)> walker;

    bool had_mission_start = false;
    bool had_mission_end   = false;
    bool had_script_start  = false;
    bool had_script_end    = false;
    int  current_cutskip   = 0;

    std::shared_ptr<Scope> current_scope = nullptr;
    bool is_condition_block = false;
    uint32_t num_statements = 0;

    auto replace_arg0 = [&](SyntaxTree& node, int32_t value)
    {
        if(node.child_count() == 2)
        {
            if(node.child(1).maybe_annotation<int32_t>().value_or(99) == 0)
            {
                node.child(1).set_annotation(value);
            }
            else
            {
                program.error(node, "XXX first argument must be 0");
            }
        }
        else
        {
            program.error(node, "XXX bad arg count");
        }
    };

    auto handle_start_new_script = [&](const SyntaxTree& node)
    {
        if(node.child_count() >= 2) // ensure XML definition is correct
        {
            //auto arg_begin = node.child_count() > 2? std::advance(node.begin(), 2) : node.end();
            auto arg_begin = node.child_count() > 2? std::next(node.begin(), 2) : node.end();
            this->send_input_vars(node.child(1), arg_begin, node.end(), program);
        }
    };

    auto handle_start_new_streamed_script = [&](const SyntaxTree& node)
    {
        // Type checking and entity passing isn't handled according to analyzes of
        // the original San Andreas SCM. TODO include a compiler flag to do so?
        // The implementation is as simple as calling handle_start_new_script :)
    };

    auto handle_script_name = [&](const SyntaxTree& node)
    {
        if(!program.opt.script_name_check)
            return;

        if(node.child_count() == 2) // ensure XML definition is correct
        {
            // why does it segfault with const TextLabelAnnotation& ???
            if(auto opt_script_name = node.child(1).maybe_annotation<const TextLabelAnnotation&>())
            {
                if(!this->script_names.emplace(opt_script_name->string).second)
                {
                    program.error(node, "XXX script name has already been used");
                }
            }
        }
    };

    auto find_command_for_expr = [&](const SyntaxTree& op) -> optional<Commands::alternator_pair>
    {
        switch(op.type())
        {
            case NodeType::Equal:
                if(is_condition_block)
                    return commands.is_thing_equal_to_thing();
                else
                    return commands.set();
            case NodeType::Cast:
                return commands.cset();
            case NodeType::Add:
                return commands.add_thing_to_thing();
            case NodeType::Sub:
                return commands.sub_thing_from_thing();
            case NodeType::Times:
                return commands.mult_thing_by_thing();
            case NodeType::Divide:
                return commands.div_thing_by_thing();
            case NodeType::TimedAdd:
                return commands.add_thing_to_thing_timed();
            case NodeType::TimedSub:
                return commands.sub_thing_from_thing_timed();
            case NodeType::Greater:
            case NodeType::Lesser: // with arguments inverted
                return commands.is_thing_greater_than_thing();
            case NodeType::GreaterEqual:
            case NodeType::LesserEqual: // with arguments inverted
                return commands.is_thing_greater_or_equal_to_thing();
            default:
                return nullopt;
        }
        Unreachable();
    };

    auto walk_on_condition = [&](SyntaxTree& node, size_t cond_child_id) {
        
        for(size_t i = 0, max = node.child_count(); i < max; ++i)
        {
            if(i == cond_child_id)
            {
                auto guard = make_scope_guard([&, was_cond_before = is_condition_block] {
                    is_condition_block = was_cond_before;
                });

                is_condition_block = true;
                node.child(i).depth_first(std::ref(walker));
            }
            else
            {
                node.child(i).depth_first(std::ref(walker));
            }
        }
    };

    walker = [&](SyntaxTree& node)
    {
        ++num_statements;
        switch(node.type()) // this switch should handle all main/top node types.
        {
            case NodeType::Label:
                // already annotated in SymTable::scan_symbols
                return false;

            case NodeType::VAR_INT: case NodeType::LVAR_INT:
            case NodeType::VAR_FLOAT: case NodeType::LVAR_FLOAT:
            case NodeType::VAR_TEXT_LABEL: case NodeType::LVAR_TEXT_LABEL:
            case NodeType::VAR_TEXT_LABEL16: case NodeType::LVAR_TEXT_LABEL16:
                // already annotated in SymTable::scan_symbols
                return false;

            case NodeType::MISSION_START:
            case NodeType::SCRIPT_START:
            {
                bool& had_start = node.type() == NodeType::MISSION_START? std::ref(had_mission_start) : 
                                  node.type() == NodeType::SCRIPT_START? std::ref(had_script_start) : Unreachable();
                auto dir_start  = node.type() == NodeType::MISSION_START? "MISSION_START" :
                                  node.type() == NodeType::SCRIPT_START? "SCRIPT_START" : Unreachable();

                if(!had_start)
                {
                    had_start = true;
                    if(num_statements != 1)
                        program.error(node, "XXX {} must be the first line of subscript or mission script.", dir_start);
                }
                else
                {
                    program.error(node, "XXX more than one {} in script.", dir_start);
                }
                return false;
            }

            case NodeType::MISSION_END:
            case NodeType::SCRIPT_END:
            {
                bool& had_start = node.type() == NodeType::MISSION_END? std::ref(had_mission_start) :
                                  node.type() == NodeType::SCRIPT_END? std::ref(had_script_start) : Unreachable();
                bool& had_end   = node.type() == NodeType::MISSION_END? std::ref(had_mission_end) :
                                  node.type() == NodeType::SCRIPT_END? std::ref(had_script_end) : Unreachable();
                auto dir_start  = node.type() == NodeType::MISSION_END? "MISSION_START" :
                                  node.type() == NodeType::SCRIPT_END? "SCRIPT_START" : Unreachable();
                auto dir_end    = node.type() == NodeType::MISSION_END? "MISSION_END" :
                                  node.type() == NodeType::SCRIPT_END? "SCRIPT_END" : Unreachable();

                if(!had_end)
                {
                    had_end = true;

                    if(commands.terminate_this_script() && commands.terminate_this_script()->supported)
                    {
                        const Command& command = *commands.terminate_this_script();
                        node.set_annotation(std::cref(command));
                    }
                    else
                    {
                        program.fatal_error(nocontext, "XXX TERMINATE_THIS_SCRIPT undefined or unsupported");
                    }

                    if(!had_start)
                        program.error(node, "XXX {} without a {}.", dir_end, dir_start);
                }
                else
                {
                    program.error(node, "XXX more than one {} in script.", dir_end);
                }
                return false;
            }

            case NodeType::Scope:
            {
                // already annotated in SymTable::scan_symbols, but let's inform about current scope
                
                auto guard = make_scope_guard([&] {
                    current_scope = nullptr;
                });

                Expects(current_scope == nullptr);

                current_scope = node.annotation<shared_ptr<Scope>>();
                node.child(0).depth_first(std::ref(walker));
                // guard sets current_scope to nullptr

                return false;
            }

            case NodeType::IF:
            {
                walk_on_condition(node, 0);
                return false;
            }

            case NodeType::WHILE:
            {
                walk_on_condition(node, 0);
                return false;
            }

            case NodeType::REPEAT:
            {
                auto& times = node.child(0);
                auto& var = node.child(1);

                // TODO cache this or dunno?
                SyntaxTree number_zero = (times.type() == NodeType::Integer? SyntaxTree::temporary(NodeType::Integer, int32_t(0)) :
                                          times.type() == NodeType::Float? SyntaxTree::temporary(NodeType::Float, float(0.0)) :
                                          (program.error(times, "XXX times must be int or float"), SyntaxTree::temporary(NodeType::Integer, int32_t(0)))); // int as fallback

                SyntaxTree number_one = (times.type() == NodeType::Integer? SyntaxTree::temporary(NodeType::Integer, int32_t(1)) :
                                         times.type() == NodeType::Float? SyntaxTree::temporary(NodeType::Float, float(1.0)) :
                                         (program.error(times, "XXX times must be int or float"), SyntaxTree::temporary(NodeType::Integer, int32_t(1)))); // int as fallback

                if(program.opt.pedantic && times.type() != NodeType::Integer)
                {
                    program.error(times, "XXX REPEAT allows only INT counters [-pedantic]");
                }

                // Walk on the REPEAT body before matching the base commands.
                // This will allow error messages in the body to be displayed even if
                // the base matching throws BadAlternator.
                node.child(2).depth_first(std::ref(walker));

                try
                {
                    const Command& set_var_to_zero = commands.match_args(symbols, current_scope, commands.set(), var, number_zero);
                    commands.annotate_args(symbols, current_scope, *this, program, set_var_to_zero, var, number_zero);
                
                    const Command& add_var_with_one = commands.match_args(symbols, current_scope, commands.add_thing_to_thing(), var, number_one);
                    commands.annotate_args(symbols, current_scope, *this, program, add_var_with_one, var, number_one);

                    const Command& is_var_geq_times = commands.match_args(symbols, current_scope, commands.is_thing_greater_or_equal_to_thing(), var, times);
                    commands.annotate_args(symbols, current_scope, *this, program, is_var_geq_times, var, times);

                    node.set_annotation(RepeatAnnotation {
                        set_var_to_zero, add_var_with_one, is_var_geq_times,
                        std::make_shared<SyntaxTree>(std::move(number_zero)),
                        std::make_shared<SyntaxTree>(std::move(number_one))
                    });
                }
                catch(const BadAlternator& e)
                {
                    program.error(e.error());
                }

                return false;
            }

            case NodeType::SWITCH:
            {
                if(!program.opt.fswitch)
                    program.error(node, "XXX SWITCH not enabled [-fswitch]");

                auto& var = node.child(0);

                auto ensure_break = [&](SyntaxTree& statement_list)
                {
                    if(statement_list.child_count() > 0)
                    {
                        auto& last_statement = statement_list.child(statement_list.child_count() - 1);
                        if(last_statement.type() == NodeType::BREAK)
                        {
                            last_statement.set_annotation(SwitchCaseBreakAnnotation {});
                            return;
                        }
                    }

                    program.error(*statement_list.parent(), "XXX CASE does not end with a BREAK");
                };

                for(auto& case_node : node.child(1))
                {
                    switch(case_node->type())
                    {
                        case NodeType::CASE:
                        {
                            try
                            {
                                auto& case_value = case_node->child(0);

                                const Command& is_var_eq_int = commands.match_args(symbols, current_scope, commands.is_thing_equal_to_thing(), var, case_value);
                                commands.annotate_args(symbols, current_scope, *this, program, is_var_eq_int, var, case_value);

                                if(!case_value.maybe_annotation<const int32_t&>())
                                    program.error(case_value, "XXX case value must be a integer constant");

                                case_node->set_annotation(SwitchCaseAnnotation { &is_var_eq_int });
                            }
                            catch(const BadAlternator& e)
                            {
                                program.error(e.error());
                            }

                            case_node->child(1).depth_first(std::ref(walker));
                            ensure_break(case_node->child(1));
                            break;
                        }
                        case NodeType::DEFAULT:
                            case_node->child(0).depth_first(std::ref(walker));
                            ensure_break(case_node->child(0));
                            break;
                        default:
                            Unreachable();
                    }
                }

                return false;
            }

            case NodeType::Command:
            {
                auto command_name = node.child(0).text();

                // TODO use `const Commands&` to identify these?
		        // TODO case sensitivity
                if(command_name == "SAVE_STRING_TO_DEBUG_FILE")
                {
                    if(commands.save_string_to_debug_file() && commands.save_string_to_debug_file()->supported)
                    {
                        const Command& command = *commands.save_string_to_debug_file();
                        if(node.child_count() < 2)
                        {
                            program.error(node, "XXX too few arguments for SAVE_STRING_TO_DEBUG_FILE");
                        }
                        else if(node.child_count() > 2)
                        {
                            program.error(node, "XXX anything but a single string argument is underspecified for SAVE_STRING_TO_DEBUG_FILE");
                        }
                        else if(node.child(1).type() != NodeType::String)
                        {
                            program.error(node.child(1), "XXX param must be a string literal");
                        }
                        else
                        {
                            auto debug_string = remove_quotes(node.child(1).text());

                            if(debug_string.size() > 127)
                            {
                                program.error(node.child(1), "XXX string too long, only 127 chars allowed");
                            }

                            node.child(1).set_annotation(String128Annotation { debug_string.to_string() });
                            node.set_annotation(std::cref(command));
                        }
                    }
                    else
                    {
                        program.fatal_error(node, "XXX SAVE_STRING_TO_DEBUG_FILE undefined or unsupported");
                    }
                }
                else if(command_name == "LOAD_AND_LAUNCH_MISSION")
                {
                    if(commands.load_and_launch_mission_internal() && commands.load_and_launch_mission_internal()->supported)
                    {
                        const Command& command = *commands.load_and_launch_mission_internal();
                        shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                        node.child(1).set_annotation(int32_t(script->mission_id.value()));
                        node.set_annotation(std::cref(command));
                    }
                    else
                    {
                        program.fatal_error(node, "XXX LOAD_AND_LAUNCH_MISSION_INTERNAL undefined or unsupported");
                    }
                }
                else if(command_name == "LAUNCH_MISSION")
                {
                    if(commands.launch_mission() && commands.launch_mission()->supported)
                    {
                        const Command& command = *commands.launch_mission();
                        shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                        node.child(1).set_annotation(script->start_label);
                        node.set_annotation(std::cref(command));
                    }
                    else
                    {
                        program.fatal_error(node, "XXX LAUNCH_MISSION undefined or unsupported");
                    }
                }
                else if(command_name == "GOSUB_FILE")
                {
                    if(commands.gosub_file() && commands.gosub_file()->supported)
                    {
                        const Command& command = *commands.gosub_file();
                        shared_ptr<Label>  label  = symbols.find_label(node.child(1).text()).value();
                        shared_ptr<Script> script = symbols.find_script(node.child(2).text()).value();
                        node.child(1).set_annotation(label);
                        node.child(2).set_annotation(script->top_label);
                        node.set_annotation(std::cref(command));
                    }
                    else
                    {
                        program.fatal_error(node, "XXX GOSUB_FILE undefined or unsupported");
                    }
                }
                else if(command_name == "REGISTER_STREAMED_SCRIPT")
                {
                    if(commands.register_streamed_script_internal() && commands.register_streamed_script_internal()->supported)
                    {
                        const Command& command = *commands.register_streamed_script_internal();
                        shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                        node.child(1).set_annotation(int32_t(script->streamed_id.value()));
                        node.set_annotation(std::cref(command));
                    }
                    else
                    {
                        program.fatal_error(node, "XXX REGISTER_STREAMED_SCRIPT_INTERNAL undefined or unsupported");
                    }
                }
                else
                {
                    // Hack to give integer-based streamed script arguments a filename based argument. Not sure if this
                    // is according to R* semantics, after all streamed scripts are all guessed. TODO rethink?
                    if(command_name == "REGISTER_SCRIPT_BRAIN_FOR_CODE_USE"
                    || command_name == "REGISTER_ATTRACTOR_SCRIPT_BRAIN_FOR_CODE_USE"
                    || command_name == "STREAM_SCRIPT"
                    || command_name == "HAS_STREAMED_SCRIPT_LOADED"
                    || command_name == "MARK_STREAMED_SCRIPT_AS_NO_LONGER_NEEDED"
                    || command_name == "REMOVE_STREAMED_SCRIPT"
                    || command_name == "REGISTER_STREAMED_SCRIPT"
                    || command_name == "START_NEW_STREAMED_SCRIPT"
                    || command_name == "GET_NUMBER_OF_INSTANCES_OF_STREAMED_SCRIPT"
                    || command_name == "ALLOCATE_STREAMED_SCRIPT_TO_RANDOM_PED"
                    || command_name == "ALLOCATE_STREAMED_SCRIPT_TO_OBJECT")
                    {
                        //|| command_name == "REGISTER_OBJECT_SCRIPT_BRAIN_FOR_CODE_USE" -- unsupported
                        //|| command_name == "ALLOCATE_STREAMED_SCRIPT_TO_PED_GENERATOR" -- unsupported
                        if(node.child_count() >= 2)
                        {
                            if(auto opt_script = symbols.find_script(node.child(1).text()))
                            {
                                if((*opt_script)->type == ScriptType::StreamedScript)
                                {
                                    auto annotation = StreamedFileAnnotation { (*opt_script)->streamed_id.value() };
                                    node.child(1).set_annotation(annotation);
                                }
                                else
                                {
                                    program.error(node.child(1), "XXX script is not a streamed script declared with REGISTER_STREAMED_SCRIPT");
                                    node.child(1).set_annotation(StreamedFileAnnotation{-1});
                                }
                            }
                            else
                            {
                                program.error(node.child(1), "XXX script never declared with REGISTER_STREAMED_SCRIPT");
                                node.child(1).set_annotation(StreamedFileAnnotation{-1});
                            }
                        }
                    }

                    try
                    {
                        const Command& command = commands.match(node, symbols, current_scope);
                        commands.annotate(node, command, symbols, current_scope, *this, program);
                        node.set_annotation(std::cref(command));

                        if(commands.equal(command, commands.start_new_script()))
                            handle_start_new_script(node);
                        else if(commands.equal(command, commands.start_new_streamed_script()))
                            handle_start_new_streamed_script(node);
                        else if(commands.equal(command, commands.script_name()))
                            handle_script_name(node);
                        else if(commands.equal(command, commands.set_collectable1_total()))
                            replace_arg0(node, symbols.count_collectable1);
                        else if(commands.equal(command, commands.set_total_number_of_missions()))
                            replace_arg0(node, symbols.count_mission_passed);
                        else if(commands.equal(command, commands.set_progress_total()))
                            replace_arg0(node, symbols.count_progress);
                        else if(commands.equal(command, commands.skip_cutscene_start()))
                        {
                            if(!program.opt.skip_cutscene)
                                program.error(node, "XXX SKIP_CUTSCENE_START not allowed [-fskip-cutscene]");

                            if(current_cutskip++ > 0)
                                program.error(node, "XXX SKIP_CUTSCENE_START inside another SKIP_CUTSCENE_START");
                            else
                                node.set_annotation(CommandSkipCutsceneStartAnnotation{});
                        }
                        else if(commands.equal(command, commands.skip_cutscene_end()))
                        {
                            if(!program.opt.skip_cutscene)
                                program.error(node, "XXX SKIP_CUTSCENE_END not allowed [-fskip-cutscene]");

                            if(current_cutskip == 0)
                                program.error(node, "XXX SKIP_CUTSCENE_END without SKIP_CUTSCENE_START");
                            else if(--current_cutskip == 0)
                                node.set_annotation(CommandSkipCutsceneEndAnnotation{});
                        }
                    }
                    catch(const BadAlternator& e)
                    {
                        program.error(e.error());
                    }
                }

                return false;
            }

            case NodeType::Equal:
            case NodeType::Cast:
            case NodeType::Greater:
            case NodeType::GreaterEqual:
            case NodeType::Lesser:
            case NodeType::LesserEqual:
            {
                try
                {
                    Commands::alternator_pair alter_cmds1 = find_command_for_expr(node).value();

                    if(auto alter_op = find_command_for_expr(node.child(1)))
                    {
                        // either 'a = b OP c' or 'a OP= c'

                        // TODO to be pedantic, alter_set can be only SET (i.e. cannot be CSET)

                        // TODO buh what if '=' is IS_THING_EQUAL_TO_THING here?

                        // TODO ensure alter_cmds1 is only '=' here (it will happen, but we need to Expects somehow)

                        const char* message = nullptr;

                        SyntaxTree& op = node.child(1);
                        SyntaxTree& a = node.child(0);
                        SyntaxTree& b = op.child(0);
                        SyntaxTree& c = op.child(1);

                        const Command& cmd_set = commands.match_args(symbols, current_scope, alter_cmds1, a, b);
                        commands.annotate_args(symbols, current_scope, *this, program, cmd_set, a, b);

                        const Command& cmd_op = commands.match_args(symbols, current_scope, *find_command_for_expr(op), a, c);
                        commands.annotate_args(symbols, current_scope, *this, program, cmd_op, a, c);

                        switch(node.child(1).type())
                        {
                            case NodeType::Sub:
                                message = "XXX cannot do VAR1 = THING - VAR1";
                                break;
                            case NodeType::Divide:
                                message = "XXX cannot do VAR1 = THING / VAR1";
                                break;
                            case NodeType::TimedAdd:
                                message = "XXX cannot do VAR1 = THING +@ VAR1";
                                break;
                            case NodeType::TimedSub:
                                message = "XXX cannot do VAR1 = THING -@ VAR1";
                                break;
                        }

                        auto a_var = a.maybe_annotation<shared_ptr<Var>>();
                        auto c_var = c.maybe_annotation<shared_ptr<Var>>();

                        if(a_var && c_var && a_var == c_var)
                        {
                            if(message)
                                program.error(node, message);

                            const Command& cmd_set2 = commands.match_args(symbols, current_scope, alter_cmds1, a, c);
                            commands.annotate_args(symbols, current_scope, *this, program, cmd_set, a, c);

                            const Command& cmd_op2 = commands.match_args(symbols, current_scope, *find_command_for_expr(op), a, b);
                            commands.annotate_args(symbols, current_scope, *this, program, cmd_op, a, b);

                            node.set_annotation(std::cref(cmd_set2));
                            op.set_annotation(std::cref(cmd_op2));
                        }
                        else
                        {
                            node.set_annotation(std::cref(cmd_set));
                            op.set_annotation(std::cref(cmd_op));
                        }
                    }
                    else
                    {
                        // 'a = b' or 'a =# b' or 'a > b' (and such)

                        bool invert = (node.type() == NodeType::Lesser || node.type() == NodeType::LesserEqual);

                        SyntaxTree& a = node.child(!invert? 0 : 1);
                        SyntaxTree& b = node.child(!invert? 1 : 0);

                        const Command& command = commands.match_args(symbols, current_scope, alter_cmds1, a, b);
                        commands.annotate_args(symbols, current_scope, *this, program, command, a, b);
                        node.set_annotation(std::cref(command));
                    }
                }
                catch(const BadAlternator& e)
                {
                    program.error(e.error());
                }
                return false;
            }

            case NodeType::Increment:
            case NodeType::Decrement:
            {
                const char* opkind     = node.type() == NodeType::Increment? "increment" : "decrement";
                auto alternator_thing  = node.type() == NodeType::Increment? commands.add_thing_to_thing() : commands.sub_thing_from_thing();

                auto& var_ident = node.child(0);
                
                if(var_ident.type() != NodeType::Identifier) // TODO unecessary? Checked on the parser.
                    program.fatal_error(var_ident, "XXX {} argument is not a identifier", opkind); // TODO use program.error and think about fallback

                // TODO array
                auto opt_varinfo = symbols.find_var(var_ident.text(), current_scope);
                if(!opt_varinfo)
                    program.fatal_error(var_ident, "XXX {} is not a variable", var_ident.text()); // TODO use program.error and think about fallback

                auto varinfo = std::move(*opt_varinfo);

                // TODO cache this or dunno?
                SyntaxTree number_one = (varinfo->type == VarType::Int? SyntaxTree::temporary(NodeType::Integer, int32_t(1)) :
                                         varinfo->type == VarType::Float? SyntaxTree::temporary(NodeType::Float, float(1.0)) :
                                         (program.error(var_ident, "XXX {} must be int or float", opkind), SyntaxTree::temporary(NodeType::Integer, int32_t(1)))); // int as fallback


                try
                {
                    const Command& op_var_with_one = commands.match_args(symbols, current_scope, alternator_thing, var_ident, number_one);
                
                    commands.annotate_args(symbols, current_scope, *this, program, op_var_with_one, var_ident, number_one);

                    node.set_annotation(IncDecAnnotation {
                        op_var_with_one,
                        std::make_shared<SyntaxTree>(std::move(number_one)),
                    });
                }
                catch(const BadAlternator& e)
                {
                    program.error(e.error());
                }

                return false;
            }

            default:
                // go into my childs to find a proper statement node
                --num_statements;
                assert(num_statements >= 0);
                return true;
        }
    };

    this->tree->depth_first(std::ref(walker));

    if(this->type == ScriptType::Mission || this->type == ScriptType::Subscript)
    {
        if(!had_mission_start)
            program.error(*this, "Mission script or subscript does not contain MISSION_START");
        else if(!had_mission_end)
            program.error(*this, "Mission script or subscript does not contain MISSION_END");
    }
    else if(this->type == ScriptType::StreamedScript)
    {
        if(!had_script_start)
            program.error(*this, "Streamed script does not contain SCRIPT_START");
        else if(!had_script_end)
            program.error(*this, "Streamed script does not contain SCRIPT_END");
    }
    else if(had_mission_start || had_script_start)
    {
        program.error(*this, "Cannot use MISSION_START or SCRIPT_START in this script type");
    }

    if(current_cutskip != 0)
    {
        program.error(*this, "XXX missing SKIP_CUTSCENE_END");
    }
}

auto read_script(const std::string& filename, const std::map<std::string, fs::path, iless>& subdir,
                 ScriptType type, ProgramContext& program) -> optional<shared_ptr<Script>>
{
    auto path_it = subdir.find(filename);
    if(path_it != subdir.end())
    {
        if(auto script_ptr = Script::create(program, path_it->second, type))
            return script_ptr;
        return nullopt;
    }
    else
    {
        program.error(nocontext, "File '{}' does not exist in '{}' subdirectory.", filename, "main");
        return nullopt;
    }
}


