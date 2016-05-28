#include "commands.hpp"
#include "symtable.hpp"
#include "error.hpp"
#include "system.hpp"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

Commands::Commands(std::multimap<std::string, Command> commands_, std::map<std::string, shared_ptr<Enum>> enums_)
    : commands(std::move(commands_)), enums(std::move(enums_))
{
    auto it_carpedmodel = this->enums.find("CARPEDMODEL");
    auto it_model       = this->enums.find("MODEL");

    if(it_carpedmodel != this->enums.end())
        this->enum_carpedmodels = it_carpedmodel->second;

    if(it_model != this->enums.end())
        this->enum_models = it_model->second;

    for(auto& pair : this->commands)
    {
        this->commands_by_id.emplace(pair.second.id, &pair.second);
    }
}

static void match_identifier_var(const shared_ptr<Var>& var, const Command::Arg& arg, const SymTable& symbols)
{
    bool is_good = false;

    if((var->global && arg.allow_global_var) || (!var->global && arg.allow_local_var))
    {
        switch(var->type)
        {
            case VarType::Int:
                is_good = (arg.type == ArgType::Integer || arg.type == ArgType::Any);
                break;
            case VarType::Float:
                is_good = (arg.type == ArgType::Float || arg.type == ArgType::Any);
                break;
            case VarType::TextLabel:
                is_good = (arg.type == ArgType::TextLabel || arg.type == ArgType::Any);
                break;
            case VarType::TextLabel16:
                // TODO ArgType::TextLabel16?? (SA)
                is_good = (arg.type == ArgType::TextLabel || arg.type == ArgType::Any);
                break;
            default:
                Unreachable();
        }

        if(is_good == false)
            throw BadAlternator("XXX type mismatch");
    }
    else
        throw BadAlternator("XXX kind of var not allowed");
}

static void match_identifier(const SyntaxTree& node, const Commands& commands, const Command::Arg& arg, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr)
{
    switch(arg.type)
    {
        case ArgType::Label:
            if(!symbols.find_label(node.text()))
                throw BadAlternator("XXX not label identifier");
            break;

        case ArgType::TextLabel:
            // Nothing to do, identifiers can be text labels with no checks.
            // TODO check for vars (SA)
            break;

        case ArgType::Integer:
        case ArgType::Float:
        case ArgType::Any:
        {
            if(arg.allow_constant && arg.type != ArgType::Float)
            {
                if(commands.find_constant_for_arg(node.text(), arg))
                    break;
            }

            if(auto opt_var = symbols.find_var(node.text(), scope_ptr))
            {
                match_identifier_var(*opt_var, arg, symbols);
                break;
            }

            throw BadAlternator("XXX");
        }

        default:
            Unreachable();
    }
}

template<typename Iter> static 
const Command& match_internal(const Commands& commands, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr,
    Commands::alternator_pair alternator_range, Iter begin, Iter end)
{
    auto num_target_args = (size_t)std::distance(begin, end);

    for(auto it = alternator_range.first; it != alternator_range.second; ++it)
    {
        const Command& alternative = it->second;

        bool is_optional = false;
        size_t args_readen = 0;

        auto it_alter_arg = alternative.args.begin();
        auto it_target_arg = begin;

        for(; ;
        (it_alter_arg->optional? it_alter_arg : ++it_alter_arg),
            ++it_target_arg,
            ++args_readen)
        {
            assert(args_readen <= num_target_args);

            if(args_readen < num_target_args)
            {
                if(it_alter_arg == alternative.args.end())
                    break; // too many args on target
            }
            else // end of arguments
            {
                if(it_alter_arg == alternative.args.end() || it_alter_arg->optional)
                    return alternative;
                else
                    break; // too few args on target
            }

            bool bad_alternative = false;

            switch((*it_target_arg)->type())
            {
                case NodeType::Integer:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::Integer) && it_alter_arg->allow_constant);
                    break;
                case NodeType::Float:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::Float) && it_alter_arg->allow_constant);
                    break;
                case NodeType::Array:
                    // TODO array SA
                    break;
                case NodeType::Identifier:
                    try
                    {
                        match_identifier(**it_target_arg, commands, *it_alter_arg, symbols, scope_ptr);
                    }
                    catch(const CompilerError&)
                    {
                        bad_alternative = true;
                    }
                    break;
                case NodeType::ShortString:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::TextLabel) && it_alter_arg->allow_constant);
                case NodeType::LongString:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::TextLabel) || argtype_matches(it_alter_arg->type, ArgType::Buffer128));
                    bad_alternative = bad_alternative || !it_alter_arg->allow_constant;;
                    break;
            }

            if(bad_alternative)
                break; // try another alternative
        }
    }

    throw BadAlternator("XXX");
}

template<typename Iter> static
void annotate_internal(const Commands& commands, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr,
    const Command& command, Iter begin, Iter end)
{
    size_t i = 0;

    // Expects all command_args (begin,end) matches command.args

    for(auto it = begin; it != end; ++it)
    {
        auto& arg = command.args[i];
        if(!arg.optional) ++i;

        SyntaxTree& arg_node = **it;

        switch((*it)->type())
        {
            case NodeType::Integer:
            {
                if(arg_node.is_annotated())
                    Expects(arg_node.maybe_annotation<const int32_t&>());
                else
                    arg_node.set_annotation(static_cast<int32_t>(std::stoi(arg_node.text(), nullptr, 0)));
                break;
            }
            case NodeType::Float:
            {
                if(arg_node.is_annotated())
                    Expects(arg_node.maybe_annotation<const float&>());
                else
                    arg_node.set_annotation(std::stof(arg_node.text()));
                break;
            }

            case NodeType::ShortString:
            case NodeType::LongString:
            {
                // TODO unescape and annotate SA
                break;
            }

            case NodeType::Array:
            {
                // TODO array SA
                break;
            }

            case NodeType::Identifier:
            {
                if(arg.type == ArgType::Label)
                {
                    if(arg_node.is_annotated())
                        Expects(arg_node.maybe_annotation<const shared_ptr<Label>&>());
                    else
                    {
                        shared_ptr<Label> label_ptr = symbols.find_label(arg_node.text()).value();
                        arg_node.set_annotation(label_ptr);
                    }
                }
                else if(arg.type == ArgType::TextLabel)
                {
                    if(arg_node.is_annotated())
                        Expects(arg_node.maybe_annotation<const std::string&>());
                    else
                        arg_node.set_annotation(arg_node.text());
                }
                else if(arg.type == ArgType::Integer || arg.type == ArgType::Float || arg.type == ArgType::Any)
                {
                    if(auto opt_const = commands.find_constant_for_arg(arg_node.text(), arg))
                    {
                        if(arg_node.is_annotated())
                            Expects(arg_node.maybe_annotation<const int32_t&>());
                        else
                            arg_node.set_annotation(*opt_const);
                    }
                    else if(auto opt_var = symbols.find_var(arg_node.text(), scope_ptr))
                    {
                        if(arg_node.is_annotated())
                            Expects(arg_node.maybe_annotation<const shared_ptr<Var>&>());
                        else
                            arg_node.set_annotation(*opt_var);
                    }
                    else
                        Unreachable();
                }
                else
                {
                    Unreachable();
                }
                break;
            }

            default:
                Unreachable();
        }
    }
}

////////

const Command& Commands::match(const SyntaxTree& command_node, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr) const
{
    auto alternator_range = commands.equal_range(command_node.child(0).text());
    return ::match_internal(*this, symbols, scope_ptr, alternator_range, command_node.begin() + 1, command_node.end());
}

const Command& Commands::match_internal(const SymTable& symbols, const shared_ptr<Scope>& scope_ptr,
    alternator_pair alternator_range, const SyntaxTree** begin, const SyntaxTree** end) const
{
    return ::match_internal(*this, symbols, scope_ptr, alternator_range, begin, end);
}

void Commands::annotate(SyntaxTree& command_node, const Command& command, 
    const SymTable& symbols, const shared_ptr<Scope>& scope_ptr) const
{
    return ::annotate_internal(*this, symbols, scope_ptr, command, command_node.begin() + 1, command_node.end());
}

void Commands::annotate_internal(const SymTable& symbols, const shared_ptr<Scope>& scope_ptr,
    const Command& command, SyntaxTree** begin, SyntaxTree** end) const
{
    return ::annotate_internal(*this, symbols, scope_ptr, command, begin, end);
}

optional<int32_t> Commands::find_constant(const std::string& value, bool context_free_only) const
{
    // TODO mayyybe speed up this? we didn't profile or anything.
    for(auto& enum_pair : enums)
    {
        if(enum_pair.second->is_global == context_free_only)
        {
            if(auto opt = enum_pair.second->find(value))
                return opt;
        }
    }
    return nullopt;
}

optional<int32_t> Commands::find_constant_for_arg(const std::string& value, const Command::Arg& arg) const
{
    if(auto opt_const = arg.find_constant(value)) // constants stricly related to this Arg
        return opt_const;

    // If the enum that the argument accepts is MODEL, and the above didn't find a match,
    // also try on the CARPEDMODEL enum.
    if(this->enum_models && this->enum_carpedmodels
     && std::find(arg.enums.begin(), arg.enums.end(), this->enum_models) != arg.enums.end())
    {
        if(auto opt_const = enum_carpedmodels->find(value))
            return opt_const;
    }

    if(auto opt_const = this->find_constant(value, true)) // global constants
        return opt_const;

    return nullopt;
}

/////////////

static int xml_stoi(const char* string)
{
    try
    {
        return std::stoi(string, nullptr, 0);
    }
    catch(const std::exception& e)
    {
        throw ConfigError("Couldn't convert string to int: {}", e.what());
    }
}

static bool xml_to_bool(const char* string)
{
    if(!strcmp(string, "true"))
        return true;
    else if(!strcmp(string, "false"))
        return false;
    else
        throw ConfigError("Boolean is not 'true' or 'false', it is '{}'", string);
}

static bool xml_to_bool(const rapidxml::xml_attribute<>* attrib, bool default_value)
{
    return attrib? xml_to_bool(attrib->value()) : default_value;
}

static ArgType xml_to_argtype(const char* string)
{
    if(!strcmp(string, "INT"))
        return ArgType::Integer;
    else if(!strcmp(string, "FLOAT"))
        return ArgType::Float;
    else if(!strcmp(string, "TEXT_LABEL"))
        return ArgType::TextLabel;
    else if(!strcmp(string, "ANY"))
        return ArgType::Any;
    else if(!strcmp(string, "LABEL"))
        return ArgType::Label;
    else if(!strcmp(string, "BUFFER"))
        return ArgType::Buffer128;
    else
        throw ConfigError("Unexpected Type attribute: {}", string);
}

static std::pair<std::string, shared_ptr<Enum>>
  parse_enum_node(const rapidxml::xml_node<>* enum_node)
{
    using namespace rapidxml;

    xml_attribute<>* enum_name_attrib   = enum_node->first_attribute("Name");
    xml_attribute<>* enum_global_attrib = enum_node->first_attribute("Global");

    if(!enum_name_attrib)
        throw ConfigError("Missing Name attrib on <Enum> node");

    std::map<std::string, int32_t> constant_map;
    int32_t current_value = 0;

    for(auto value_node = enum_node->first_node(); value_node; value_node = value_node->next_sibling())
    {
        assert(!strcmp(value_node->name(), "Constant"));

        xml_attribute<>* value_name_attrib = value_node->first_attribute("Name");
        xml_attribute<>* value_value_attrib = value_node->first_attribute("Value");

        if(!value_name_attrib)
            throw ConfigError("Missing Name attrib on <Constant> node");

        if(value_value_attrib)
            current_value = xml_stoi(value_value_attrib->value());

        constant_map.emplace(value_name_attrib->value(), current_value);

        ++current_value;
    }

    auto enump = std::make_shared<Enum>(Enum {
        std::move(constant_map),
        xml_to_bool(enum_global_attrib, false),
    });

    return { enum_name_attrib->value(), std::move(enump) };
}

static std::pair<std::string, Command>
  parse_command_node(const rapidxml::xml_node<>* cmd_node, const std::map<std::string, shared_ptr<Enum>>& enums)
{
    using namespace rapidxml;

    xml_attribute<>* id_attrib   = cmd_node->first_attribute("ID");
    xml_attribute<>* name_attrib = cmd_node->first_attribute("Name");
    xml_attribute<>* support_attrib = cmd_node->first_attribute("Supported");
    xml_node<>*      args_node   = cmd_node->first_node("Args");

    if(!id_attrib || !name_attrib)
        throw ConfigError("Missing ID or Name attrib on <Command> node");

    std::vector<Command::Arg> args;

    if(args_node)
    {
        size_t num_args = 0;

        // Avoid realocations, find out the number of args beforehand.
        for(auto x = args_node->first_node(); x; x = x->next_sibling())
            ++num_args;

        args.reserve(num_args);

        for(auto arg_node = args_node->first_node("Arg"); arg_node; arg_node = arg_node->next_sibling("Arg"))
        {
            auto type_attrib         = arg_node->first_attribute("Type");
            auto out_attrib          = arg_node->first_attribute("Out");
            auto ref_attrib          = arg_node->first_attribute("Ref");
            auto optional_attrib     = arg_node->first_attribute("Optional");
            auto allow_const_attrib  = arg_node->first_attribute("AllowConst");
            auto allow_gvar_attrib   = arg_node->first_attribute("AllowGlobalVar");
            auto allow_lvar_attrib   = arg_node->first_attribute("AllowLocalVar");
            auto entity_attrib       = arg_node->first_attribute("Entity");
            auto enum_attrib         = arg_node->first_attribute("Enum");

            if(!type_attrib)
                throw ConfigError("Missing Type attrib on <Arg> node");

            // TODO OUT
            // TODO REF
            // TODO ENTITY
            // Cannot build with a initializer list, VS goes mad :(
            Command::Arg arg;
            arg.type = xml_to_argtype(type_attrib->value());
            arg.optional = xml_to_bool(optional_attrib, false);
            arg.allow_constant = xml_to_bool(allow_const_attrib, true);
            arg.allow_global_var= xml_to_bool(allow_gvar_attrib, true);
            arg.allow_local_var = xml_to_bool(allow_lvar_attrib, true);

            if(enum_attrib)
            {
                auto eit = enums.find(enum_attrib->value());
                if(eit != enums.end())
                {
                    arg.enums.emplace_back(eit->second);
                    arg.enums.shrink_to_fit();
                }
            }
            
            args.emplace_back(std::move(arg));
        }
    }
    
    return {
        name_attrib->value(),
        Command {
            xml_to_bool(support_attrib, true),               // supported
            uint16_t(xml_stoi(id_attrib->value()) & 0x7FFF), // id
            std::move(args),                                 // args
        }
    };
}

Commands Commands::from_xml(const std::vector<fs::path>& xml_list)
{
    using namespace rapidxml;

    std::multimap<std::string, Command>     commands;
    std::map<std::string, shared_ptr<Enum>> enums;

    fs::path conf_path = config_path();

    for(auto& xml_path : xml_list)
    {
        std::string xml_data;
        xml_document<> doc;   // xml_data should be alive as long as doc

        fs::path full_xml_path(conf_path / xml_path);
        try
        {
            xml_data = read_file_utf8(full_xml_path).value();

            if(xml_data.empty())
                continue;

            doc.parse<0>(&xml_data[0]); // xml_data will get modified here
        }
        catch(const rapidxml::parse_error& e)
        {
            throw ConfigError("Failed to parse XML {}: {}", full_xml_path.generic_u8string(), e.what());
        }
        catch(const bad_optional_access& e)
        {
            throw ConfigError("Failed to read XML {}: {}", full_xml_path.generic_u8string(), e.what());
        }

        if(xml_node<>* root_node = doc.first_node("GTA3Script"))
        {
            for(auto node = root_node->first_node(); node; node = node->next_sibling())
            {
                if(!strcmp(node->name(), "Commands"))
                {
                    for(auto cmd_node = node->first_node(); cmd_node; cmd_node = cmd_node->next_sibling())
                    {
                        if(!strcmp(cmd_node->name(), "Command"))
                        {
                            auto cmd_pair = parse_command_node(cmd_node, enums);
                            commands.emplace(std::move(cmd_pair));
                        }
                    }
                }
                else if(!strcmp(node->name(), "Constants"))
                {
                    for(auto const_node = node->first_node(); const_node; const_node = const_node->next_sibling())
                    {
                        if(!strcmp(const_node->name(), "Enum"))
                        {
                            auto enum_pair = parse_enum_node(const_node);
                            enums.emplace(std::move(enum_pair));
                        }
                    }
                }
            }
        }

    }

    return Commands(std::move(commands), std::move(enums));
}
