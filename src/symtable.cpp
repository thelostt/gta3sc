#include <stdinc.h>
#include "symtable.hpp"
#include "commands.hpp"
#include "program.hpp"
#include "codegen.hpp"

// TODO check if vars, labels, etc aren't already constants and etc

// TODO android compiler registers START_CUTSCENE stuff before the global variable space

// TODO this source needs to be separated in a few others

static auto get_base_var_annotation(const SyntaxTree& var_node) -> optional<shared_ptr<Var>>
{
    if(auto opt = var_node.maybe_annotation<const shared_ptr<Var>&>())
        return *opt;
    else if(auto opt = var_node.maybe_annotation<const ArrayAnnotation&>())
        return opt->base;
    else
        return nullopt;
}

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

auto Scope::output_type_from_node(const SyntaxTree& node) -> optional<OutputType>
{
    if(auto opt_var = get_base_var_annotation(node))
        return (*opt_var)->type;
    else if(node.maybe_annotation<const int32_t&>())
        return OutputType::Int;
    else if(node.maybe_annotation<const float&>())
        return OutputType::Float;
    else if(node.maybe_annotation<const TextLabelAnnotation&>())
        return OutputType::TextLabel;
    else
        return nullopt;
}

bool Label::may_branch_from(const Script& other_script, ProgramContext& program) const
{
    if(!this->script->uses_local_offsets())
        return true;
    return this->script->on_the_same_space_as(other_script);
}

void Script::add_children(shared_ptr<Script> script)
{
    Expects(script.get() != this);
    Expects(script->type == ScriptType::Required);
    Expects(script->parent_script.use_count() == 0);

    this->children_scripts.emplace_back(script);
    script->parent_script = this->shared_from_this();
}

bool Script::uses_local_offsets() const
{
    switch(this->type)
    {
        case ScriptType::Main:
        case ScriptType::MainExtension:
        case ScriptType::Subscript:
            return false;
        case ScriptType::Mission:
        case ScriptType::StreamedScript:
        case ScriptType::CustomMission:
        case ScriptType::CustomScript:
            return true;
        case ScriptType::Required:
            return this->root_script()->uses_local_offsets();
        default:
            Unreachable();
    }
}

bool Script::on_the_same_space_as(const Script& other) const
{
    if(this == &other)
        return true;

    if(this->type == ScriptType::Required)
        return this->root_script()->on_the_same_space_as(other);

    if(other.type == ScriptType::Required)
        return this->on_the_same_space_as(*other.root_script());

    if(this->type == ScriptType::Main
    || this->type == ScriptType::MainExtension
    || this->type == ScriptType::Subscript)
    {
        switch(other.type)
        {
            case ScriptType::Main:
            case ScriptType::MainExtension:
            case ScriptType::Subscript:
                return true;
            default:
                return false;
        }
    }
    else if(this->type == ScriptType::Mission
            || this->type == ScriptType::StreamedScript
            || this->type == ScriptType::CustomScript
            || this->type == ScriptType::CustomMission)
    {
        return this == &other;
    }
    else
    {
        Unreachable();
    }
}

bool Script::is_child_of(ScriptType type) const
{
    Expects(type != ScriptType::Required);
    if(this->type == ScriptType::Required)
        return this->root_script()->is_child_of(type);
    else
        return this->type == type;
}

bool Script::is_child_of_mission() const
{
    return is_child_of(ScriptType::Mission) || is_child_of(ScriptType::CustomMission);
}

bool Script::is_child_of_custom() const
{
    return is_child_of(ScriptType::CustomScript) || is_child_of(ScriptType::CustomMission);
}

shared_ptr<const Script> Script::root_script() const
{
    if(!this->is_root_script())
        return this->parent_script.lock()->root_script();
    else
        return this->shared_from_this();
}

bool Script::is_root_script() const
{
    return !(this->type == ScriptType::Required);
}

size_t Script::distance_from_root() const
{
    if(this->is_root_script())
        return 0;

    auto parent = this->parent_script.lock();
    return parent->code_size.value() + parent->distance_from_root();
}

void Script::compute_script_offsets(const std::vector<shared_ptr<Script>>& scripts, const MultiFileHeaderList& headers)
{
    size_t offset = 0;
    for(auto& script_ptr : scripts)
    {
        assert(script_ptr->base == nullopt && script_ptr->code_offset == nullopt);
        script_ptr->base.emplace(offset);
        offset += headers.compiled_size(script_ptr);
        script_ptr->code_offset.emplace(offset);
        offset += script_ptr->code_size.value();
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

void Script::handle_special_commands(const std::vector<shared_ptr<Script>>& scripts, SymTable& symbols, ProgramContext& program)
{
    std::vector<std::pair<shared_ptr<const Scope>, size_t>> scope_num_inputs;
    shared_ptr<const Scope> last_scope_entered;

    auto handle_script_input = [&program](const SyntaxTree& arg_node, const shared_ptr<Var>& lvar, bool is_cleo_call) -> bool
    {
        if(auto opt_arg_var = get_base_var_annotation(arg_node))
        {
            auto& argvar = *opt_arg_var;

            if(argvar->type != lvar->type
            && !(is_cleo_call && argvar->is_text_var() && lvar->type == VarType::Int))
            {
                program.error(arg_node, "type mismatch in target label");
                // TODO more info
                return false;
            }

            if(lvar->entity && argvar->entity != lvar->entity)
            {
                program.error(arg_node, "entity type mismatch in target label");
                // TODO more info
                return false;
            }

            lvar->entity = argvar->entity;
            return true;
        }
        else
        {
            program.error(arg_node, "type mismatch in target label");
            // TODO more info
            return false;
        }
    };

    auto send_input_vars = [&](const SyntaxTree::const_iterator& input_begin,
                               const SyntaxTree::const_iterator& input_end,
                               shared_ptr<const Scope>& target_scope, bool is_cleo_call)
    {
        size_t target_var_index = 0;
        for(auto arginput = input_begin; arginput != input_end; ++arginput)
        {
            auto lvar = target_scope->var_at(target_var_index);

            if(!lvar)
            {
                program.error(**arginput, "not enough local variables in target label");
                break;
            }

            switch(lvar->type)
            {
                case VarType::Int:
                    ++target_var_index;
                    if((**arginput).maybe_annotation<const int32_t&>())
                        break;
                    if(is_cleo_call && (**arginput).maybe_annotation<const TextLabelAnnotation&>())
                        break;
                    handle_script_input(**arginput, lvar, is_cleo_call);
                    break;
                case VarType::Float:
                    ++target_var_index;
                    if((**arginput).maybe_annotation<const float&>())
                        break;
                    handle_script_input(**arginput, lvar, is_cleo_call);
                    break;
                case VarType::TextLabel:
                    target_var_index += 2;
                    program.error(**arginput, "type mismatch in target label because LVAR_TEXT_LABEL is not allowed");
                    break;
                case VarType::TextLabel16:
                    target_var_index += 4;
                    program.error(**arginput, "type mismatch in target label because LVAR_TEXT_LABEL16 is not allowed");
                    break;
                default:
                    Unreachable();
            }
        }
    };

    auto recv_output_vars = [&](const SyntaxTree::const_iterator& output_begin,
                                const SyntaxTree::const_iterator& output_end,
                                shared_ptr<const Scope>& target_scope, bool is_cleo_call)
    {
        Expects(is_cleo_call == true);

        size_t i = 0;
        for(auto argoutput = output_begin; argoutput != output_end; ++argoutput)
        {
            if(auto opt_outvar = get_base_var_annotation(**argoutput))
            {
                auto& outvar       = *opt_outvar;
                auto& scope_output = (*target_scope->outputs)[i];
                auto output_type   = scope_output.first;
                auto output_entity = scope_output.second.expired()? 0 : scope_output.second.lock()->entity;

                Expects(output_type == Scope::OutputType::Int || output_type == Scope::OutputType::Float);

                if(output_type != outvar->type)
                {
                    program.error(**argoutput, "type mismatch");
                    // TODO more info
                    continue;
                }

                if(outvar->entity && output_entity != outvar->entity)
                {
                    program.error(**argoutput, "entity type mismatch");
                    // TODO more info
                    continue;
                }

                outvar->entity = output_entity;
            }
            else
            {
                program.error(**argoutput, "expected a variable as output");
            }
        }
    };

    auto handle_start_new_script = [&](const SyntaxTree& node, const Command& command)
    {
        if(!program.opt.entity_tracking || node.child_count() < 2)
            return;

        auto& arglabel_node = node.child(1);

        auto opt_target_label = arglabel_node.maybe_annotation<const shared_ptr<Label>&>();
        if(!opt_target_label)
        {
            program.warning(arglabel_node, "target label is not a label identifier");
            return;
        }

        auto& target_scope = (*opt_target_label)->scope;
        if(!target_scope)
        {
            auto where = program.opt.scope_then_label? "after" : "before";
            program.error(arglabel_node, "expected scope in target label");
            program.note(arglabel_node, "add a '{{' {} the target label", where);
            return;
        }

        send_input_vars(std::next(node.begin(), 2), node.end(), target_scope, false);
    };

    auto handle_start_new_streamed_script = [&](const SyntaxTree& node, const Command& command)
    {
        if(!program.opt.entity_tracking || node.child_count() < 2)
            return;

        // TODO
        return;
    };

    auto handle_entity_command = [&](const SyntaxTree& node, const Command& command)
    {
        if(!program.opt.entity_tracking)
            return;

        size_t i = 0;
        for(auto it = std::next(node.begin()); it != node.end(); ++it, ++i)
        {
            auto& arginfo = command.arg(i).value();
            if(arginfo.entity_type != 0)
            {
                if(auto opt_argvar = get_base_var_annotation(**it))
                {
                    auto& argvar = **opt_argvar;
                    if(arginfo.is_output)
                    {
                        if(argvar.entity && argvar.entity != arginfo.entity_type)
                        {
                            auto type_old = program.commands.find_entity_name(argvar.entity).value();
                            auto type_new = program.commands.find_entity_name(arginfo.entity_type).value();
                            program.error(**it, "variable has already been used to create a entity of type {}", type_old);
                        }

                        argvar.entity = arginfo.entity_type;
                    }
                    else
                    {
                        if(argvar.entity != arginfo.entity_type)
                        {
                            auto expect = program.commands.find_entity_name(arginfo.entity_type).value();
                            auto got    = program.commands.find_entity_name(argvar.entity).value();
                            program.error(**it, "expected variable of type {} but got {}", expect, got);
                        }
                    }
                }
            }
        }
    };
    
    auto handle_cleo_call = [&](SyntaxTree& node, const Command& command)
    {
        if(node.child_count() < 3)
            return;

        auto& arglabel_node = node.child(1);
        auto& argcount_node = node.child(2);

        auto opt_target_label = arglabel_node.maybe_annotation<const shared_ptr<Label>&>();
        if(!opt_target_label)
        {
            program.warning(arglabel_node, "target label is not a label identifier");
            return;
        }

        auto& target_scope = (*opt_target_label)->scope;
        if(!target_scope)
        {
            auto where = program.opt.scope_then_label? "after" : "before";
            program.error(arglabel_node, "expected scope in target label");
            program.note(arglabel_node, "add a '{{' {} the target label", where);
            return;
        }

        if(!target_scope->is_call_scope())
        {
            program.error(arglabel_node, "target scope does not have a CLEO_RETURN");
            return;
        }

        if(argcount_node.maybe_annotation<int32_t>().value_or(99) != 0)
            program.error(argcount_node, "this argument shall be set to 0");

        auto& outputs     = *target_scope->outputs;
        size_t num_args   = node.child_count() - 3;

        if(num_args < outputs.size())
        {
            program.error(node, "too few arguments for this function call");
            return;
        }

        size_t num_inputs  = num_args - outputs.size();

        // TODO maybe store it in the scope object
        auto it_num_input = std::find_if(scope_num_inputs.begin(), scope_num_inputs.end(), [&](const auto& v) {
            return v.first == target_scope;
        });
        
        if(it_num_input == scope_num_inputs.end())
        {
            scope_num_inputs.emplace_back(std::make_pair(target_scope, num_inputs));
        }
        else if(num_inputs < it_num_input->second)
        {
            program.error(node, "too few arguments for this function call");
            return;
        }
        else if(num_inputs > it_num_input->second)
        {
            program.error(node, "too many arguments for this function call");
            return;
        }

        send_input_vars(std::next(node.begin(), 3), std::next(node.begin(), 3+num_inputs), target_scope, true);
        recv_output_vars(std::next(node.begin(), 3+num_inputs), node.end(), target_scope, true);

        argcount_node.set_annotation(int32_t(num_inputs));
    };

    auto handle_cleo_return = [&](SyntaxTree& node, const Command& command)
    {
        if(node.child_count() < 2)
            return;

        auto& outputs = last_scope_entered->outputs.value();

        size_t num_outputs = node.child_count() - 2;

        auto& argcount_node = node.child(1);

        if(argcount_node.maybe_annotation<int32_t>().value_or(99) != 0)
            program.error(argcount_node, "this argument shall be set to 0");

        argcount_node.set_annotation(int32_t(num_outputs));

        if(num_outputs > outputs.size())
        {
            program.error(node, "too many return arguments");
            return;
        }
        else if(num_outputs < outputs.size())
        {
            program.error(node, "too few return arguments");
            return;
        }

        size_t i = 0;
        for(auto it = std::next(node.begin(), 2); it != node.end(); ++it, ++i)
        {
            auto& output = outputs[i];
            if(auto opt_type = Scope::output_type_from_node(**it))
            {
                if(*opt_type != output.first)
                {
                    program.error(**it, "type mismatch");
                    // TODO more info
                    continue;
                }

                if(auto opt_var = get_base_var_annotation(**it))
                {
                    if(!output.second.expired())
                    {
                        auto output_var = output.second.lock();
                        auto& return_var = *opt_var;

                        if(output_var->entity != return_var->entity)
                        {
                            program.error(**it, "entity type mismatch");
                            // TODO more info
                            continue;
                        }
                    }
                }
            }
            else
            {
                program.error(node, "unrecognized output type");
            }
        }
    };

    for(auto& script : scripts)
    {
        script->tree->depth_first([&](SyntaxTree& node)
        {
            switch(node.type())
            {
                case NodeType::Scope:
                {
                    // any scope checking already happened at this point,
                    // so no need for handling entering/exiting, just handle scopes being reached
                    last_scope_entered = node.annotation<shared_ptr<Scope>>();
                    return true;
                }

                case NodeType::Command:
                {
                    if(auto opt_command = node.maybe_annotation<std::reference_wrapper<const Command>>())
                    {
                        auto& command = (*opt_command).get();
                        if(program.commands.equal(command, program.commands.start_new_script))
                            handle_start_new_script(node, command);
                        else if(program.commands.equal(command, program.commands.start_new_streamed_script))
                            handle_start_new_streamed_script(node, command);
                        else if(program.commands.equal(command, program.commands.cleo_call))
                            handle_cleo_call(node, command);
                        else if(program.commands.equal(command, program.commands.cleo_return))
                            handle_cleo_return(node, command);
                        else
                            handle_entity_command(node, command);
                    }
                    return false;
                }

                case NodeType::Equal:
                {
                    auto& a = node.child(0);
                    auto& b = node.child(1);

                    // handle only a = b, not a = b op c (TODO that should be handled as well).
                    if(!b.maybe_annotation<std::reference_wrapper<const Command>>())
                    {
                        auto& command = node.annotation<std::reference_wrapper<const Command>>().get();
                        if(program.commands.is_alternator(command, program.commands.set))
                        {
                            auto opt_avar = get_base_var_annotation(a);
                            auto opt_bvar = get_base_var_annotation(b);
                            if(opt_avar && opt_bvar)
                            {
                                auto& avar = **opt_avar;
                                auto& bvar = **opt_bvar;

                                if(avar.entity && avar.entity != bvar.entity)
                                {
                                    auto type_a = program.commands.find_entity_name(avar.entity).value();
                                    auto type_b = program.commands.find_entity_name(bvar.entity).value();
                                    program.error(node, "assignment of variable of type {} into one of type {}", type_b, type_a);
                                }

                                avar.entity = bvar.entity;
                            }
                        }
                    }

                    return false;
                }

                default:
                    return true;
            }
        });
    }
}

void Script::compute_scope_outputs(const SymTable& symbols, ProgramContext& program)
{
    if(!program.opt.cleo)
        return;

    for(auto& scope : this->scopes)
    {
        scope->tree.lock()->depth_first([&](const SyntaxTree& node)
        {
            if(scope->outputs) // already found
                return false;

            switch(node.type())
            {
                case NodeType::Command:
                {
                    // TODO use const Commands& instead of comparing strings
                    if(node.child(0).text() == "CLEO_RETURN")
                    {
                        if(node.child_count() < 2)
                        {
                            program.error(node, "CLEO_RETURN must have at least one parameter, which is a 0 placeholder");
                            return false;
                        }

                        scope->outputs.emplace();
                        auto& outputs = *scope->outputs;

                        for(auto it = std::next(node.begin(), 2); it != node.end(); ++it)
                        {
                            if((*it)->type() == NodeType::Text)
                            {
                                auto opt_match = Miss2Identifier::match((*it)->text());
                                if(opt_match)
                                {
                                    string_view varname;
                                    if(opt_match->identifier.front() == '$')
                                        varname = opt_match->identifier.substr(1);
                                    else
                                        varname = opt_match->identifier;

                                    if(auto opt_var = symbols.find_var(varname, scope))
                                    {
                                        if((*opt_var)->is_text_var() && opt_match->identifier.front() != '$')
                                            outputs.emplace_back(Scope::OutputType::TextLabel, *opt_var);
                                        else
                                            outputs.emplace_back((*opt_var)->type, *opt_var);
                                    }
                                    else
                                        outputs.emplace_back(Scope::OutputType::TextLabel, weak_ptr<Var>());

                                    if(outputs.back().first == Scope::OutputType::TextLabel)
                                        program.error(**it, "this output type is not supported");
                                }
                                else
                                {
                                    program.error(**it, ::to_string(opt_match.error()));
                                }
                            }
                            else if((*it)->type() == NodeType::Integer)
                            {
                                outputs.emplace_back(Scope::OutputType::Int, weak_ptr<Var>());
                            }
                            else if((*it)->type() == NodeType::Float)
                            {
                                outputs.emplace_back(Scope::OutputType::Float, weak_ptr<Var>());
                            }
                            else if((*it)->type() == NodeType::String)
                            {
                                program.error(**it, "this output type is not supported");
                            }
                            else
                            {
                                program.error(**it, "unrecognized output type");
                            }
                        }
                    }
                    return false;
                }

                default:
                    return true;
            }
        });
    }
}

void Script::fix_call_scope_variables(ProgramContext& program)
{
    if(program.opt.mission_var_begin == 0 || !this->is_child_of_mission())
        return;

    for(auto& scope : this->scopes)
    {
        if(scope->is_call_scope())
        {
            for(auto& var : scope->vars)
            {
                auto& var_index = var.second->index;

                if(var_index >= uint32_t(program.opt.timer_index) && var_index <= uint32_t(program.opt.timer_index+1))
                    continue;

                Expects(var_index >= program.opt.mission_var_begin);
                var_index -= program.opt.mission_var_begin;
            }
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
        program.error(nocontext, "SET_PROGRESS_TOTAL happens multiple times between script units");
    if(this->count_set_collectable1_total > 1)
        program.error(nocontext, "SET_COLLECTABLE1_TOTAL happens multiple times between script units");
    if(this->count_set_total_number_of_missions > 1)
        program.error(nocontext, "SET_TOTAL_NUMBER_OF_MISSIONS happens multiple times between script units");
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
                program.error(**sc1, "duplicate script name '{}'", inpair.first->first);
                program.note(*inpair.first->second, "previously used here");
            }
        }
    }
}

/// Gets script type from filename.
auto SymTable::script_type(const string_view& filename) const -> optional<ScriptType>
{
    auto searcher = [&](const std::string& other) {
        return iequal_to()(other, filename);
    };

    if(std::any_of(this->extfiles.begin(), this->extfiles.end(), searcher))
        return ScriptType::MainExtension;
    else if(std::any_of(this->subscript.begin(), this->subscript.end(), searcher))
        return ScriptType::Subscript;
    else if(std::any_of(this->mission.begin(), this->mission.end(), searcher))
        return ScriptType::Mission;
    else if(std::any_of(this->streamed.begin(), this->streamed.end(), searcher))
        return ScriptType::StreamedScript;
    else if(std::any_of(this->required.begin(), this->required.end(), searcher))
        return ScriptType::Required;
    else
        return nullopt;
}

bool SymTable::add_script(ScriptType type, const SyntaxTree& command, ProgramContext& program)
{
    size_t num_required_args = (type == ScriptType::MainExtension? 2 :
                                type == ScriptType::StreamedScript? 2 : 1);

    if(command.child_count() != 1+num_required_args)
    {
        // TODO what's the error message for normal commands?
        program.error(command, "bad number of arguments");
        return false;
    }
    else
    {
        // TODO R* compiler checks if parameter ends with .sc
        // TODO check command.child_count() is ok with name_child_id 

        size_t name_child_id = num_required_args;

        auto script_name = command.child(name_child_id).text();
        auto existing_type = this->script_type(script_name);

        if(existing_type)
        {
            if(type != *existing_type)
            {
                program.error(command, "incompatible declaration, script was previously seen as a {} script", to_string(*existing_type));
                return false;
            }
            return true;
        }
        else 
        {
            auto refvector = (type == ScriptType::MainExtension? std::ref(this->extfiles) :
                type == ScriptType::Subscript? std::ref(this->subscript) :
                type == ScriptType::Mission? std::ref(this->mission) :
                type == ScriptType::StreamedScript? std::ref(this->streamed) :
                type == ScriptType::Required? std::ref(this->required) : Unreachable());

            if(type == ScriptType::StreamedScript)
            {
                this->streamed_names.emplace_back(command.child(1).text());
            }

            refvector.get().emplace_back(script_name);
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
            auto script1 = t1.labels.find(kv.first)->second->script;
            auto script2 = t2.labels.find(kv.first)->second->script;
            program.error(*script2, "duplicate label '{}'", kv.first);
            program.note(*script1, "previously seen here");
        }
    }

    if(int_gvars.size() > 0)
    {
        for(auto& kv : int_gvars)
        {
            // TODO maybe store in Var the Script where it was declared?
            program.error(nocontext, "duplicate global variable '{}' between script units", kv.first);
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

    t1.required.reserve(t1.required.size() + t2.required.size());
    std::move(t2.required.begin(), t2.required.end(), std::back_inserter(t1.required));

    t1.extfiles.reserve(t1.extfiles.size() + t2.extfiles.size());
    std::move(t2.extfiles.begin(), t2.extfiles.end(), std::back_inserter(t1.extfiles));

    t1.subscript.reserve(t1.subscript.size() + t2.subscript.size());
    std::move(t2.subscript.begin(), t2.subscript.end(), std::back_inserter(t1.subscript));

    t1.mission.reserve(t1.mission.size() + t2.mission.size());
    std::move(t2.mission.begin(), t2.mission.end(), std::back_inserter(t1.mission));

    t1.streamed.reserve(t1.streamed.size() + t2.streamed.size());
    std::move(t2.streamed.begin(), t2.streamed.end(), std::back_inserter(t1.streamed));

    t1.streamed_names.reserve(t1.streamed_names.size() + t2.streamed_names.size());
    std::move(t2.streamed_names.begin(), t2.streamed_names.end(), std::back_inserter(t1.streamed_names));

    t1.count_progress += t2.count_progress;
    t1.count_collectable1 += t2.count_collectable1;
    t1.count_mission_passed += t2.count_mission_passed;

    t1.count_set_progress_total += t2.count_set_progress_total;
    t1.count_set_collectable1_total += t2.count_set_collectable1_total;
    t1.count_set_total_number_of_missions += t2.count_set_total_number_of_missions;
}

void SymTable::scan_for_includers(Script& script, ProgramContext& program)
{
    std::function<bool(SyntaxTree&)> walker;
    SymTable& table = *this;

    auto add_script = [&](ScriptType type, const SyntaxTree& command)
    {
        if(type == ScriptType::Required && script.type == ScriptType::Required)
        {
            program.error(command, "using REQUIRE inside a required file is forbidden");
        }
        else if(type == ScriptType::Required || script.type == ScriptType::Main || script.type == ScriptType::MainExtension)
        {
            table.add_script(type, command, program);
        }
        else if(script.is_child_of_custom())
        {
            program.error(command, "this command is not allowed in custom scripts");
        }
        else
        {
            auto bad_message = "use of this command outside main or extension files is not well-defined";
            if(type == ScriptType::Mission || type == ScriptType::StreamedScript)
                program.error(command, bad_message);
            else
                program.warning(command, bad_message);
        }
    };

    // the scanner
    walker = [&](SyntaxTree& node)
    {
        switch(node.type())
        {
            case NodeType::Command:
            {
                auto command_name = node.child(0).text();

                // TODO use `const Commands&` to identify these?
                // TODO case sensitivity
                if(command_name == "LOAD_AND_LAUNCH_MISSION")
                    add_script(ScriptType::Mission, node);
                else if(command_name == "LAUNCH_MISSION")
                    add_script(ScriptType::Subscript, node);
                else if(command_name == "GOSUB_FILE")
                    add_script(ScriptType::MainExtension, node);
                else if(command_name == "REGISTER_STREAMED_SCRIPT")
                    add_script(ScriptType::StreamedScript, node);
                else if(command_name == "REQUIRE")
                {
                    if(program.opt.pedantic)
                        program.error(node, "REQUIRE is a language extension [-pedantic]");

                    add_script(ScriptType::Required, node);
                }

                return false;
            }

            default:
                return true;
        }
    };

    script.tree->depth_first(std::ref(walker));
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
                // Just emit a warning since R* compiler accepts this construct.
                program.warning(node, "progress value is not a constant");
                return 0;
            }
        }
        else
        {
            // TODO what's the error message for normal commands?
            program.error(node, "bad number of arguments");
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
                        opt_label_ptr = table.find_label(label_name).value();
                        program.error(node, "redefinition of label '{}'", label_name);
                        program.note(*(*opt_label_ptr)->script, "previous definition is here"); 
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
                    program.error(node, "already inside a scope");
                    // continue using current scope instead of entering a new one
                }
                else
                {
                    local_index = (!script.is_child_of_mission()? 0 : program.opt.mission_var_begin);
                    current_scope = table.add_scope(node);
                    current_scope->vars.emplace("TIMERA", std::make_shared<Var>(false, VarType::Int, program.opt.timer_index + 0, nullopt));
                    current_scope->vars.emplace("TIMERB", std::make_shared<Var>(false, VarType::Int, program.opt.timer_index + 1, nullopt));
                    script.scopes.emplace_back(current_scope);
                }

                if(next_scoped_label)
                {
                    auto label_name = next_scoped_label->text();
                    
                    if(program.opt.pedantic && program.opt.scope_then_label)
                    {
                        program.error(node, "since Vice City the label should be inside the scope [-pedantic]");
                    }

                    auto opt_label_ptr = table.add_label(label_name.to_string(), current_scope, script.shared_from_this());
                    if(!opt_label_ptr)
                    {
                        opt_label_ptr = table.find_label(label_name).value();
                        program.error(node, "redefinition of label '{}'", label_name);
                        program.note(*(*opt_label_ptr)->script, "previous definition is here");
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
                if(program.opt.output_cleo && command_name == "START_NEW_SCRIPT")
                    program.error(node, "this command is not allowed in custom scripts");
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
                    program.error(node, "{} happens more than once", command_name);

                return false;
            }

            case NodeType::VAR_INT: case NodeType::LVAR_INT:
            case NodeType::VAR_FLOAT: case NodeType::LVAR_FLOAT:
            case NodeType::VAR_TEXT_LABEL: case NodeType::LVAR_TEXT_LABEL:
            case NodeType::VAR_TEXT_LABEL16: case NodeType::LVAR_TEXT_LABEL16:
            {
                bool global; VarType vartype;

                std::tie(global, vartype) = token_to_vartype(node.type());

                if(global && program.opt.output_cleo)
                {
                    program.error(node, "declaring global variables in custom scripts isn't allowed");
                    return false;
                }

                if(!global && current_scope == nullptr)
                {
                    program.error(node, "local variable definition outside of scope");
                    return false;
                }

                if(!program.opt.text_label_vars
                    && (vartype == VarType::TextLabel || vartype == VarType::TextLabel16))
                {
                    program.error(node, "text label variables are not supported");
                }

                auto& target = global? table.global_vars : current_scope->vars;
                auto& index = global? global_index : local_index;
                size_t max_index;
                
                if(global)
                    max_index = (65536 / 4);
                else if(script.is_child_of_mission())
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
                            if(is<size_t>(*token.index))
                            {
                                count = get<size_t>(*token.index);
                                if(*count == 0)
                                {
                                    program.error(varnode, "declaring a zero-sized array");
                                    count = 1; // fallback
                                }
                                else if(program.opt.array_elem_limit && *count > *program.opt.array_elem_limit)
                                {
                                    auto elem_limit = *program.opt.array_elem_limit;
                                    program.error(varnode, "arrays are limited to a maximum of {} elements", elem_limit);
                                }
                            }
                            else
                            {
                                // TODO allow enum?
                                program.error(varnode, "index must be constant");
                                count = 1; // fallback
                            }
                        }
                    }
                    else
                    {
                        program.error(varnode, to_string(opt_token.error()));
                        continue;
                    }

                    if(!program.opt.farrays && count)
                    {
                        program.error(varnode, "arrays are not supported [-farrays]");
                    }

                    auto it = target.emplace(name, std::make_shared<Var>(global, vartype, index, count));
                    auto var_ptr = it.first->second;

                    if(it.second == false)
                    {
                        program.error(varnode, "redefinition of variable '{}'", name);
                        varnode.set_annotation(std::move(var_ptr));
                    }
                    else
                    {
                        index += var_ptr->space_taken();
                        varnode.set_annotation(std::move(var_ptr));
                    }

                    if(index > max_index)
                        program.error(varnode, "reached maximum {} variable limit ('{}')", (global? "global" : "local"), max_index);
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
    uint32_t num_directives = 0;

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
                program.error(node, "first argument must be 0");
                program.note(node, "the compiler will tweak the value automatically");
            }
        }
        else
        {
            // TODO what's the error message for normal commands?
            program.error(node, "bad number of arguments");
        }
    };

    auto handle_script_name = [&](const SyntaxTree& node)
    {
        if(!program.opt.script_name_check)
            return;

        if(node.child_count() == 2) // ensure XML definition is correct
        {
            if(auto opt_script_name = node.child(1).maybe_annotation<const TextLabelAnnotation&>())
            {
                if(!this->script_names.emplace(opt_script_name->string).second)
                {
                    program.error(node, "duplicate script name '{}'", opt_script_name->string);
                }
            }
        }
    };

    auto find_alternator_for_expr = [&](const SyntaxTree& op) -> optional<const Commands::Alternator&>
    {
        switch(op.type())
        {
            case NodeType::Equal:
                if(is_condition_block)
                    return commands.is_thing_equal_to_thing;
                else
                    return commands.set;
            case NodeType::Cast:
                return commands.cset;
            case NodeType::Add:
                return commands.add_thing_to_thing;
            case NodeType::Sub:
                return commands.sub_thing_from_thing;
            case NodeType::Times:
                return commands.mult_thing_by_thing;
            case NodeType::Divide:
                return commands.div_thing_by_thing;
            case NodeType::TimedAdd:
                return commands.add_thing_to_thing_timed;
            case NodeType::TimedSub:
                return commands.sub_thing_from_thing_timed;
            case NodeType::Greater:
            case NodeType::Lesser: // with arguments inverted
                return commands.is_thing_greater_than_thing;
            case NodeType::GreaterEqual:
            case NodeType::LesserEqual: // with arguments inverted
                return commands.is_thing_greater_or_equal_to_thing;
            default:
                return nullopt;
        }
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
                ++num_directives;

                bool& had_start = node.type() == NodeType::MISSION_START? std::ref(had_mission_start) : 
                                  node.type() == NodeType::SCRIPT_START? std::ref(had_script_start) : Unreachable();
                auto dir_start  = node.type() == NodeType::MISSION_START? "MISSION_START" :
                                  node.type() == NodeType::SCRIPT_START? "SCRIPT_START" : Unreachable();

                if(!had_start)
                {
                    had_start = true;
                    if(num_statements != 1)
                        program.error(node, "{} must be the first statement in script", dir_start);
                }
                else
                {
                    program.error(node, "more than one {} directive in script", dir_start);
                }
                return false;
            }

            case NodeType::MISSION_END:
            case NodeType::SCRIPT_END:
            {
                ++num_directives;

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

                    if(this->is_child_of(ScriptType::CustomScript))
                    {
                        const Command& command = program.supported_or_fatal(node, commands.terminate_this_custom_script, "TERMINATE_THIS_CUSTOM_SCRIPT");
                        node.set_annotation(std::cref(command));
                    }
                    else
                    {
                        const Command& command = program.supported_or_fatal(node, commands.terminate_this_script, "TERMINATE_THIS_SCRIPT");
                        node.set_annotation(std::cref(command));
                    }

                    if(!had_start)
                        program.error(node, "{} without a {}", dir_end, dir_start);
                }
                else
                {
                    program.error(node, "more than one {} in script", dir_end);
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

                auto number_zero = (times.type() == NodeType::Integer? Commands::MatchArgument(0) :
                                    times.type() == NodeType::Float? Commands::MatchArgument(0.0f) :
                                    (program.error(times, "REPEAT counter must be INT or FLOAT"), Commands::MatchArgument(0))); // int as fallback

                auto number_one  = (times.type() == NodeType::Integer? Commands::MatchArgument(1) :
                                    times.type() == NodeType::Float? Commands::MatchArgument(1.0f) :
                                    (program.error(times, "REPEAT counter must be INT or FLOAT"), Commands::MatchArgument(0))); // int as fallback

                if(program.opt.pedantic && times.type() != NodeType::Integer)
                {
                    program.error(times, "REPEAT only allows INT counters [-pedantic]");
                }

                // Walk on the REPEAT body before matching the base commands.
                // This will allow error messages in the body to be displayed even if
                // the base matching throws BadAlternator.
                node.child(2).depth_first(std::ref(walker));

                auto& alt_set                                = program.supported_or_fatal(node, commands.set, "SET");
                auto& alt_add_thing_to_thing                 = program.supported_or_fatal(node, commands.add_thing_to_thing,
                                                                                          "ADD_THING_TO_THING");
                auto& alt_is_thing_greater_or_equal_to_thing = program.supported_or_fatal(node, commands.is_thing_greater_or_equal_to_thing,
                                                                                          "IS_THING_GREATER_OR_EQUAL_TO_THING");

                auto exp_set_var_to_zero = commands.match(alt_set, node, { &var, number_zero }, symbols, current_scope);
                auto exp_add_var_with_one = commands.match(alt_add_thing_to_thing, node, { &var, number_one }, symbols, current_scope);
                auto exp_is_var_geq_times = commands.match(alt_is_thing_greater_or_equal_to_thing, node, { &var, &times }, symbols, current_scope);

                if(exp_set_var_to_zero && exp_add_var_with_one && exp_is_var_geq_times)
                {
                    commands.annotate({ &var, nullopt }, **exp_set_var_to_zero, symbols, current_scope, *this, program);
                    commands.annotate({ &var, nullopt }, **exp_add_var_with_one, symbols, current_scope, *this, program);
                    commands.annotate({ &var, &times }, **exp_is_var_geq_times, symbols, current_scope, *this, program);

                    node.set_annotation(RepeatAnnotation {
                        **exp_set_var_to_zero, **exp_add_var_with_one, **exp_is_var_geq_times,
                        std::move(number_zero), std::move(number_one)
                    });
                }
                else
                {
                    if(!exp_set_var_to_zero)  exp_set_var_to_zero.error().emit(program);
                    if(!exp_add_var_with_one) exp_add_var_with_one.error().emit(program);
                    if(!exp_is_var_geq_times) exp_is_var_geq_times.error().emit(program);
                }

                return false;
            }

            case NodeType::SWITCH:
            {
                if(!program.opt.fswitch)
                    program.error(node, "SWITCH not supported [-fswitch]");

                auto& var = node.child(0);
                bool last_statement_was_break = true;
                shared_ptr<const SyntaxTree> last_case;

                std::vector<int32_t> case_values;
                bool had_default = false;

                for(auto& body_node : node.child(1))
                {
                    switch(body_node->type())
                    {
                        case NodeType::CASE:
                        {
                            if(last_case && !last_statement_was_break)
                                program.error(*last_case, "CASE does not end with a BREAK");

                            auto& case_node = body_node;
                            last_case       = body_node;

                            auto& case_value = case_node->child(0);

                            auto& alt_is_thing_equal_to_thing = program.supported_or_fatal(node, commands.is_thing_equal_to_thing,
                                                                                            "IS_THING_EQUAL_TO_THING");

                            auto exp_is_var_eq_int  = commands.match(alt_is_thing_equal_to_thing, *case_node, { &var, &case_value }, symbols, current_scope);
                            if(exp_is_var_eq_int)
                            {
                                commands.annotate({ &var, &case_value }, **exp_is_var_eq_int, symbols, current_scope, *this, program);

                                if(auto v = case_value.maybe_annotation<const int32_t&>())
                                {
                                    if(std::find(case_values.begin(), case_values.end(), *v) != case_values.end())
                                        program.error(*case_node, "duplicate CASE value '{}'", *v);
                                    else
                                        case_values.emplace_back(*v);
                                }
                                else
                                {
                                    program.error(*case_node, "CASE value must be a integer constant");
                                }

                                case_node->set_annotation(SwitchCaseAnnotation { *exp_is_var_eq_int });
                            }
                            else
                            {
                                if(exp_is_var_eq_int.error().reason == Commands::MatchFailure::NoAlternativeMatch)
                                    program.error(*case_node, "type mismatch between SWITCH variable and CASE value");
                                else
                                    exp_is_var_eq_int.error().emit(program);
                            }

                            break;
                        }

                        case NodeType::DEFAULT:
                        {
                            if(had_default)
                                program.error(*body_node, "multiple DEFAULT labels in one SWITCH");

                            if(last_case && !last_statement_was_break)
                                program.error(*last_case, "CASE does not end with a BREAK");

                            last_case = body_node;
                            had_default = true;
                            break;
                        }

                        case NodeType::BREAK:
                            if(last_case)
                            {
                                last_statement_was_break = true;
                            }
                            else
                            {
                                program.error(*body_node, "BREAK not within a CASE or DEFAULT label");
                            }
                            break;

                        default: // statement
                            if(last_case)
                            {
                                last_statement_was_break = false;
                                body_node->depth_first(std::ref(walker));
                            }
                            else
                            {
                                program.error(*body_node, "statement not within a CASE or DEFAULT label");
                            }
                            break;
                    }
                }

                if(last_case && !last_statement_was_break)
                    program.error(*last_case, "CASE does not end with a BREAK");

                if(auto switch_case_limit = program.opt.switch_case_limit)
                {
                    if(case_values.size() > static_cast<size_t>(*switch_case_limit))
                        program.error(node, "SWITCH contains more than {} cases [-fswitch-case-limit]", *switch_case_limit);
                }

                node.set_annotation(SwitchAnnotation { case_values.size(), had_default });

                return false;
            }

            case NodeType::Command:
            {
                auto command_name = node.child(0).text();
                auto use_filenames = (this->type == ScriptType::Main || this->type == ScriptType::MainExtension);


                // TODO use `const Commands&` to identify these?
		        // TODO case sensitivity
                if(command_name == "SAVE_STRING_TO_DEBUG_FILE")
                {
                    const Command& command = program.supported_or_fatal(node, commands.save_string_to_debug_file,
                                                                        "SAVE_STRING_TO_DEBUG_FILE");
                    if(node.child_count() < 2)
                    {
                        // TODO what's the error message for normal commands?
                        program.error(node, "too few arguments");
                    }
                    else if(node.child_count() > 2)
                    {
                        // TODO what's the error message for normal commands?
                        program.error(node, "anything but a single STRING argument is underspecified for SAVE_STRING_TO_DEBUG_FILE");
                    }
                    else if(node.child(1).type() != NodeType::String)
                    {
                        // TODO what's the error message for normal commands?
                        program.error(node.child(1), "argument must be a STRING literal");
                    }
                    else
                    {
                        auto debug_string = remove_quotes(node.child(1).text());

                        if(debug_string.size() > 127)
                        {
                            program.error(node.child(1), "STRING is too long, only 127 characters allowed");
                        }

                        node.child(1).set_annotation(String128Annotation { debug_string.to_string() });
                        node.set_annotation(std::cref(command));
                    }
                }
                else if(use_filenames && command_name == "LOAD_AND_LAUNCH_MISSION")
                {
                    const Command& command = program.supported_or_fatal(node, commands.load_and_launch_mission_internal,
                                                                        "LOAD_AND_LAUNCH_MISSION_INTERNAL");
                    shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                    //node.child(1).set_annotation(int32_t(script->mission_id.value()));
                    //node.set_annotation(std::cref(command));
                    node.set_annotation(ReplacedCommandAnnotation { std::cref(command), { int32_t(script->mission_id.value()) } });
                }
                else if(use_filenames && command_name == "LAUNCH_MISSION")
                {
                    const Command& command = program.supported_or_fatal(node, commands.launch_mission,
                                                                        "LAUNCH_MISSION");
                    shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                    node.child(1).set_annotation(script->start_label);
                    node.set_annotation(std::cref(command));
                }
                else if(use_filenames && command_name == "GOSUB_FILE")
                {
                    const Command& command = program.supported_or_fatal(node, commands.gosub_file,
                                                                        "GOSUB_FILE");
                    shared_ptr<Label>  label  = symbols.find_label(node.child(1).text()).value();
                    node.child(1).set_annotation(label);
                    node.child(2).set_annotation(label);
                    node.set_annotation(std::cref(command));
                }
                else if(use_filenames && command_name == "REGISTER_STREAMED_SCRIPT")
                {
                    const Command& command = program.supported_or_fatal(node, commands.register_streamed_script_internal,
                                                                        "REGISTER_STREAMED_SCRIPT_INTERNAL");
                    auto streamed_id = symbols.find_streamed_id(node.child(1).text()).value();
                    //node.child(1).set_annotation(int32_t(streamed_id));
                    node.set_annotation(ReplacedCommandAnnotation { std::cref(command), { int32_t(streamed_id) } });
                }
                else if(command_name == "REQUIRE")
                {
                    const Command& command = program.supported_or_fatal(node, commands.require, "REQUIRE");
                    shared_ptr<Script> script = symbols.find_script(node.child(1).text()).value();
                    node.child(1).set_annotation(script->top_label);
                    node.set_annotation(DummyCommandAnnotation{});
                }
                else
                {
                    auto exp_command = commands.match(node, symbols, current_scope);
                    if(exp_command)
                    {
                        const Command& command = **exp_command;

                        commands.annotate(node, command, symbols, current_scope, *this, program);
                        node.set_annotation(std::cref(command));

                        if(!command.supported)
                            program.error(node, "unsupported command");

                        if(command.internal)
                            program.error(node, "unexpected use of internal command");

                        if(commands.equal(command, commands.script_name))
                            handle_script_name(node);
                        else if(commands.equal(command, commands.set_collectable1_total))
                            replace_arg0(node, symbols.count_collectable1);
                        else if(commands.equal(command, commands.set_total_number_of_missions))
                            replace_arg0(node, symbols.count_mission_passed);
                        else if(commands.equal(command, commands.set_progress_total))
                            replace_arg0(node, symbols.count_progress);
                        else if(commands.equal(command, commands.terminate_this_script) && program.opt.output_cleo && !program.opt.mission_script)
                            program.error(node, "command not allowed in custom scripts, please use TERMINATE_THIS_CUSTOM_SCRIPT");
                        else if(commands.equal(command, commands.terminate_this_custom_script) && (!program.opt.output_cleo || program.opt.mission_script))
                            program.error(node, "command not allowed in multifile scripts or custom missions, please use TERMINATE_THIS_SCRIPT");
                        else if(commands.equal(command, commands.skip_cutscene_start))
                        {
                            if(!program.opt.skip_cutscene)
                                program.error(node, "SKIP_CUTSCENE_START not supported [-fskip-cutscene]");

                            if(current_cutskip++ > 0)
                                program.error(node, "SKIP_CUTSCENE_START inside another SKIP_CUTSCENE_START");
                            else
                                node.set_annotation(CommandSkipCutsceneStartAnnotation{});
                        }
                        else if(commands.equal(command, commands.skip_cutscene_end))
                        {
                            if(!program.opt.skip_cutscene)
                                program.error(node, "SKIP_CUTSCENE_END not supported [-fskip-cutscene]");

                            if(current_cutskip == 0)
                                program.error(node, "SKIP_CUTSCENE_END without SKIP_CUTSCENE_START");
                            else if(--current_cutskip == 0)
                                node.set_annotation(CommandSkipCutsceneEndAnnotation{});
                        }
                    }
                    else
                    {
                        exp_command.error().emit(program);
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
                const Commands::Alternator& alter_cmds1 = program.supported_or_fatal(node, find_alternator_for_expr(node), "<unknown>");

                if(auto alter_op = find_alternator_for_expr(node.child(1)))
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

                    auto exp_cmd_set = commands.match(alter_cmds1, node, { &a, &b }, symbols, current_scope);
                    auto exp_cmd_op  = commands.match(*alter_op, node, { &a, &c }, symbols, current_scope);

                    if(exp_cmd_set && exp_cmd_op)
                    {
                        commands.annotate({ &a, &b }, **exp_cmd_set, symbols, current_scope, *this, program);
                        commands.annotate({ &a, &c }, **exp_cmd_op, symbols, current_scope, *this, program);

                        switch(op.type())
                        {
                            case NodeType::Sub:
                                message = "cannot do VAR1 = THING - VAR1";
                                break;
                            case NodeType::Divide:
                                message = "cannot do VAR1 = THING / VAR1";
                                break;
                            case NodeType::TimedAdd:
                                message = "cannot do VAR1 = THING +@ VAR1";
                                break;
                            case NodeType::TimedSub:
                                message = "cannot do VAR1 = THING -@ VAR1";
                                break;
                        }

                        auto a_var = a.maybe_annotation<shared_ptr<Var>>();
                        auto b_var = b.maybe_annotation<shared_ptr<Var>>();
                        auto c_var = c.maybe_annotation<shared_ptr<Var>>();

                        if(a_var && c_var && a_var == c_var) // Y = THING op Y
                        {
                            if(message)
                            {
                                if(a_var != b_var || b_var != c_var) // allow Y = Y / Y
                                    program.error(node, message);
                            }

                            auto exp_cmd_set2 = commands.match(alter_cmds1, node, { &a, &c }, symbols, current_scope);
                            auto exp_cmd_op2  = commands.match(*alter_op, node, { &a, &b }, symbols, current_scope);

                            if(exp_cmd_set2 && exp_cmd_op2)
                            {
                                commands.annotate({ &a, &c }, **exp_cmd_set2, symbols, current_scope, *this, program);
                                commands.annotate({ &a, &b }, **exp_cmd_op2, symbols, current_scope, *this, program);

                                node.set_annotation(std::cref(**exp_cmd_set2));
                                op.set_annotation(std::cref(**exp_cmd_op2));
                            }
                            else
                            {
                                if(!exp_cmd_set2) exp_cmd_set2.error().emit(program);
                                if(!exp_cmd_op2) exp_cmd_op2.error().emit(program);
                            }
                        }
                        else
                        {
                            node.set_annotation(std::cref(**exp_cmd_set));
                            op.set_annotation(std::cref(**exp_cmd_op));
                        }
                    }
                    else
                    {
                        if(!exp_cmd_set) exp_cmd_set.error().emit(program);
                        if(!exp_cmd_op) exp_cmd_op.error().emit(program);
                    }
                }
                else
                {
                    // 'a = b' or 'a =# b' or 'a > b' (and such)

                    bool invert = (node.type() == NodeType::Lesser || node.type() == NodeType::LesserEqual);

                    SyntaxTree& a = node.child(!invert? 0 : 1);
                    SyntaxTree& b = node.child(!invert? 1 : 0);

                    auto exp_command = commands.match(alter_cmds1, node, { &a, &b }, symbols, current_scope);
                    if(exp_command)
                    {
                        commands.annotate({ &a, &b }, **exp_command, symbols, current_scope, *this, program);
                        node.set_annotation(std::cref(**exp_command));
                    }
                    else
                    {
                        exp_command.error().emit(program);
                    }
                }
                return false;
            }

            case NodeType::Increment:
            case NodeType::Decrement:
            {
                const char* opkind     = node.type() == NodeType::Increment? "increment" : "decrement";
                auto alternator_thing0  = node.type() == NodeType::Increment? commands.add_thing_to_thing : commands.sub_thing_from_thing;

                auto& alternator_thing = program.supported_or_fatal(node, alternator_thing0, "<unknown>");

                auto& var_ident = node.child(0);
                
                // TODO array
                auto opt_varinfo = symbols.find_var(var_ident.text(), current_scope);
                if(!opt_varinfo)
                    program.fatal_error(var_ident, "'{}' is not a variable", var_ident.text()); // TODO use program.error and think about fallback

                auto varinfo = std::move(*opt_varinfo);

                if(varinfo->type == VarType::Int)
                {
                    auto exp_op_var_with_one = commands.match(alternator_thing, node, { &var_ident, 1 }, symbols, current_scope);
                    if(exp_op_var_with_one)
                    {
                        commands.annotate({ &var_ident, nullopt }, **exp_op_var_with_one, symbols, current_scope, *this, program);

                        node.set_annotation(IncDecAnnotation {
                            **exp_op_var_with_one, 1,
                        });
                    }
                    else
                    {
                        exp_op_var_with_one.error().emit(program);
                    }
                }
                else
                {
                    program.error(var_ident, "{} operand must be of type INT", opkind);
                }

                return false;
            }

            case NodeType::BREAK:
                if(!program.opt.allow_break_continue)
                    program.error(node, "BREAK only allowed at the end of a SWITCH CASE [-fbreak-continue]");
                return false;

            case NodeType::CONTINUE:
                if(!program.opt.allow_break_continue)
                    program.error(node, "CONTINUE is not supported [-fbreak-continue]");
                return false;

            case NodeType::DUMP:
                if(program.opt.pedantic)
                    program.error(node, "DUMP is a language extension [-pedantic]");
                return false;

            default:
                // go into my childs to find a proper statement node
                --num_statements;
                assert(num_statements >= 0);
                return true;
        }
    };

    this->tree->depth_first(std::ref(walker));

    if(this->type == ScriptType::Mission || this->type == ScriptType::CustomMission || this->type == ScriptType::Subscript)
    {
        if(!had_mission_start)
            program.error(*this, "{} script does not contain MISSION_START", to_string(this->type));
        else if(!had_mission_end)
            program.error(*this, "{} script does not contain MISSION_END", to_string(this->type));
    }
    else if(this->type == ScriptType::CustomScript || this->type == ScriptType::StreamedScript)
    {
        if(!had_script_start)
            program.error(*this, "{} script does not contain SCRIPT_START", to_string(this->type));
        else if(!had_script_end)
            program.error(*this, "{} script does not contain SCRIPT_END", to_string(this->type));
    }
    else if(had_mission_start || had_script_start)
    {
        program.error(*this, "cannot use {} in {} scripts",
                      had_mission_start? "MISION_START" : "SCRIPT_START",
                      to_string(this->type));
    }

    if(current_cutskip != 0)
    {
        program.error(*this, "missing SKIP_CUTSCENE_END");
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
        program.error(nocontext, "file '{}' does not exist in the scripts subdirectory", filename);
        return nullopt;
    }
}


