#include "symtable.hpp"
#include "commands.hpp"

// TODO check if vars, labels, etc aren't already constants and etc

// TODO android compiler registers START_CUTSCENE stuff before the global variable space

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
        else 
        {
            // still not added to its list?
            if(!found_extfile && !found_subscript && !found_mission)
            {
                auto& vector = (type == ScriptType::MainExtension? std::ref(this->extfiles) :
                    type == ScriptType::Subscript? std::ref(this->subscript) :
                    type == ScriptType::Mission? std::ref(this->mission) : Unreachable());

                vector.get().emplace_back(std::move(script_name));
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

    // TODO check for conflicting extfiles, subscripts and mission

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

    t1.count_progress += t2.count_progress;
    t1.count_collectable1 += t2.count_collectable1;
    t1.count_mission_passed += t2.count_mission_passed;

    t1.count_set_progress_total += t2.count_set_progress_total;
    t1.count_set_collectable1_total += t2.count_set_collectable1_total;
    t1.count_set_total_number_of_missions += t2.count_set_total_number_of_missions;
}

void SymTable::scan_symbols(Script& script, const Commands& commands, ProgramContext& program)
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
                return std::stoi(node.child(1).text());
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
                    
                    auto opt_label_ptr = table.add_label(label_name, current_scope, script.shared_from_this());
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
                    // TODO timer indices are different on SA
                    // TODO should we cache the shared_ptr of the timers to avoid allocation on each scope?
                    current_scope->vars.emplace("TIMERA", std::make_shared<Var>(false, VarType::Int, 16, nullopt));
                    current_scope->vars.emplace("TIMERB", std::make_shared<Var>(false, VarType::Int, 17, nullopt));
                }

                if(next_scoped_label)
                {
                    auto& label_name = next_scoped_label->child(0).text();
                    
                    auto opt_label_ptr = table.add_label(label_name, current_scope, script.shared_from_this());
                    if(!opt_label_ptr)
                    {
                        program.error(node, "XXX Label {} already exists", label_name);
                        opt_label_ptr = table.find_label(label_name).value();
                    }

                    Expects(opt_label_ptr != nullopt);
                    next_scoped_label->set_annotation(std::move(*opt_label_ptr));
                    next_scoped_label = nullptr;
                }

                node.walk(std::ref(walker));

                node.set_annotation(std::move(current_scope));

                // guard sets current_scope to previous value (probably nullptr)

                return false;
            }

            case NodeType::Command:
            {
                auto& command_name = node.child(0).text();

                uint16_t count_unique_command = 0;

                // TODO use `const Commands&` to identify these?
                if(command_name == "LOAD_AND_LAUNCH_MISSION")
                    table.add_script(ScriptType::Mission, node, program);
                else if(command_name == "LAUNCH_MISSION")
                    table.add_script(ScriptType::Subscript, node, program);
                else if(command_name == "GOSUB_FILE")
                    table.add_script(ScriptType::MainExtension, node, program);
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
                // TODO where should we put the global/local variable limitations? Here?

                bool global; VarType vartype;

                std::tie(global, vartype) = token_to_vartype(node.type());

                if(!global && current_scope == nullptr)
                {
                    program.error(node, "XXX Local var definition outside scope.");
                    return false;
                }

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
                        {
                            program.error(varnode, "XXX Negative or zero array counter {}.", name);
                            array_counter = 1; // fallback to 1 instead of halting compilation
                        }

                        count = array_counter;
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
                }

                return false;
            }

            default:
                return true;
        }
    };

    script.tree->walk(std::ref(walker));
}

void Script::annotate_tree(const SymTable& symbols, const Commands& commands, ProgramContext& program)
{
    std::function<bool(SyntaxTree&)> walker;

    bool had_mission_start = false;
    bool had_mission_end   = false;

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
            case NodeType::Plus:
                return commands.add_thing_to_thing();
            case NodeType::Minus:
                return commands.sub_thing_from_thing();
            case NodeType::Times:
                return commands.mult_thing_by_thing();
            case NodeType::Divide:
                return commands.div_thing_by_thing();
            case NodeType::TimedPlus:
                return commands.add_thing_to_thing_timed();
            case NodeType::TimedMinus:
                return commands.sub_thing_from_thing_timed();
            case NodeType::Greater:
            case NodeType::Lesser: // with arguments inverted
                return commands.is_thing_greater_than_thing();
            case NodeType::GreaterEqual:
            case NodeType::LesserEqual: // with arguments inverted
                return commands.is_thing_greater_or_equal_to_thing();
            case NodeType::Module:
                // TODO CLEO
                break;
            case NodeType::LeftShift:
                // TODO CLEO
                break;
            case NodeType::RightShift:
                // TODO CLEO
                break;
            case NodeType::BitAND:
                // TODO CLEO
                break;
            case NodeType::BitOR:
                // TODO CLEO
                break;
            case NodeType::BitXOR:
                // TODO CLEO
                break;
            case NodeType::OR:
                // TODO CLEO
                break;
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
                node.child(i).walk_inclusive(std::ref(walker));
            }
            else
            {
                node.child(i).walk_inclusive(std::ref(walker));
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
            {
                if(!had_mission_start)
                {
                    had_mission_start = true;

                    if(num_statements != 1)
                        program.error(node, "XXX MISSION_START must be the first line of subscript or mission script.");
                }
                else
                {
                    program.error(node, "XXX more than one MISSION_START in script.");
                }
                return false;
            }

            case NodeType::MISSION_END:
            {
                if(!had_mission_end)
                {
                    had_mission_end = true;
                    const Command& command = commands.terminate_this_script();
                    node.set_annotation(std::cref(command));

                    if(!had_mission_start)
                        program.error(node, "XXX MISSION_END without a MISSION_START.");
                }
                else
                {
                    program.error(node, "XXX more than one MISSION_END in script.");
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
                node.walk(std::ref(walker));
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
                // TODO to be pedantic REPEAT must accept only INT times
                SyntaxTree number_zero = (times.type() == NodeType::Integer? SyntaxTree::temporary(NodeType::Integer, "0") :
                                          times.type() == NodeType::Float? SyntaxTree::temporary(NodeType::Float, "0.0") :
                                          (program.error(times, "XXX times must be int or float"), SyntaxTree::temporary(NodeType::Integer, "0"))); // int as fallback

                SyntaxTree number_one = (times.type() == NodeType::Integer? SyntaxTree::temporary(NodeType::Integer, "1") :
                                         times.type() == NodeType::Float? SyntaxTree::temporary(NodeType::Float, "1.0") :
                                         (program.error(times, "XXX times must be int or float"), SyntaxTree::temporary(NodeType::Integer, "1"))); // int as fallback

                // Walk on the REPEAT body before matching the base commands.
                // This will allow error messages in the body to be displayed even if
                // the base matching throws BadAlternator.
                node.child(2).walk(std::ref(walker));

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

            case NodeType::Command:
            {
                auto& command_name = node.child(0).text();

                // TODO use `const Commands&` to identify these?
                if(command_name == "LOAD_AND_LAUNCH_MISSION")
                {
                    const Command& command = commands.load_and_launch_mission();
                    shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                    node.child(1).set_annotation(int32_t(script->mission_id.value()));
                    node.set_annotation(std::cref(command));
                }
                else if(command_name == "LAUNCH_MISSION")
                {
                    const Command& command = commands.launch_mission();
                    shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                    node.child(1).set_annotation(script->start_label);
                    node.set_annotation(std::cref(command));
                }
                else if(command_name == "GOSUB_FILE")
                {
                    const Command& command = commands.gosub_file();
                    shared_ptr<Label>  label  = symbols.find_label(node.child(1).text()).value();
                    shared_ptr<Script> script = symbols.find_script(node.child(2).text()).value();
                    node.child(1).set_annotation(label);
                    node.child(2).set_annotation(script->top_label);
                    node.set_annotation(std::cref(command));
                }
                else
                {
                    try
                    {
                        const Command& command = commands.match(node, symbols, current_scope);
                        commands.annotate(node, command, symbols, current_scope, *this, program);
                        node.set_annotation(std::cref(command));

                        if(commands.equal(command, commands.set_collectable1_total()))
                            replace_arg0(node, symbols.count_collectable1);
                        else if(commands.equal(command, commands.set_total_number_of_missions()))
                            replace_arg0(node, symbols.count_mission_passed);
                        else if(commands.equal(command, commands.set_progress_total()))
                            replace_arg0(node, symbols.count_progress);
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

                        SyntaxTree& op = node.child(1);
                        SyntaxTree& a = node.child(0);
                        SyntaxTree& b = op.child(0);
                        SyntaxTree& c = op.child(1);

                        const Command& cmd_set = commands.match_args(symbols, current_scope, alter_cmds1, a, b);
                        commands.annotate_args(symbols, current_scope, *this, program, cmd_set, a, b);

                        const Command& cmd_op = commands.match_args(symbols, current_scope, *find_command_for_expr(op), a, c);
                        commands.annotate_args(symbols, current_scope, *this, program, cmd_op, a, c);

                        node.set_annotation(std::cref(cmd_set));
                        op.set_annotation(std::cref(cmd_op));
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
                auto& alternator_thing = node.type() == NodeType::Increment? commands.add_thing_to_thing() : commands.sub_thing_from_thing();

                auto& var_ident = node.child(0);

                if(var_ident.type() != NodeType::Identifier) // TODO unecessary? Checked on the parser.
                    program.fatal_error(var_ident, "XXX {} argument is not a identifier", opkind); // TODO use program.error and think about fallback

                auto opt_varinfo = symbols.find_var(var_ident.text(), current_scope);
                if(!opt_varinfo)
                    program.fatal_error(var_ident, "XXX {} is not a variable", var_ident.text()); // TODO use program.error and think about fallback

                auto varinfo = std::move(*opt_varinfo);

                // TODO cache this or dunno?
                SyntaxTree number_one = (varinfo->type == VarType::Int? SyntaxTree::temporary(NodeType::Integer, "1") :
                                         varinfo->type == VarType::Float? SyntaxTree::temporary(NodeType::Float, "1.0") :
                                         (program.error(var_ident, "XXX {} must be int or float", opkind), SyntaxTree::temporary(NodeType::Integer, "1")) ); // int as fallback


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

    this->tree->walk(std::ref(walker));

    if(this->type == ScriptType::Mission || this->type == ScriptType::Subscript)
    {
        if(!had_mission_start)
            program.error(*this, "Mission script or subscript does not contain MISSION_START");
        else if(!had_mission_end)
            program.error(*this, "Mission script or subscript does not contain MISSION_END");
    }
}


