#include <stdinc.h>
#include "commands.hpp"
#include "program.hpp"
#include "system.hpp"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

static int xml_stoi(const char* string)
{
    try
    {
        return std::stoi(string, nullptr, 0);
    }
    catch(const std::exception& e)
    {
        throw ConfigError("couldn't convert string to int: {}", e.what());
    }
}

static bool xml_to_bool(const char* string)
{
    if(!strcmp(string, "true"))
        return true;
    else if(!strcmp(string, "false"))
        return false;
    else
        throw ConfigError("boolean is not 'true' or 'false', it is '{}'", string);
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
    else if(!strcmp(string, "PARAM"))
        return ArgType::Param;
    else if(!strcmp(string, "LABEL"))
        return ArgType::Label;
    else if(!strcmp(string, "BUFFER32"))
        return ArgType::Buffer32;
    else if(!strcmp(string, "CONST"))
        return ArgType::Constant;
    else if(!strcmp(string, "TEXT_LABEL"))
        return ArgType::TextLabel;
    else if(!strcmp(string, "TEXT_LABEL16"))
        return ArgType::TextLabel16;
    else if(!strcmp(string, "ANY_TEXT_LABEL")) // TODO remove me?
        return ArgType::AnyTextLabel;
    else if(!strcmp(string, "STRING"))
        return ArgType::AnyTextLabel;
    else
        throw ConfigError("unexpected 'Type' attribute: {}", string);
}

static void parse_enum_node(transparent_map<std::string, shared_ptr<Enum>>& enums, const rapidxml::xml_node<>* enum_node)
{
    using namespace rapidxml;

    xml_attribute<>* enum_name_attrib   = enum_node->first_attribute("Name");
    xml_attribute<>* enum_global_attrib = enum_node->first_attribute("Global");

    if(!enum_name_attrib)
        throw ConfigError("missing 'Name' attribute on '<Enum>' node");

    bool is_global = xml_to_bool(enum_global_attrib, false);
    auto eit = enums.find(enum_name_attrib->value());
    if(eit == enums.end())
    {
        auto enum_ptr = std::make_shared<Enum>(Enum { {}, is_global });
        eit = enums.emplace(enum_name_attrib->value(), std::move(enum_ptr)).first;
    }
    else
    {
        assert(is_global == eit->second->is_global);
    }

    insensitive_map<std::string, int32_t>& constant_map = eit->second->values;
    int32_t current_value = 0;

    for(auto value_node = enum_node->first_node(); value_node; value_node = value_node->next_sibling())
    {
        assert(!strcmp(value_node->name(), "Constant"));

        xml_attribute<>* value_name_attrib = value_node->first_attribute("Name");
        xml_attribute<>* value_value_attrib = value_node->first_attribute("Value");

        if(!value_name_attrib)
            throw ConfigError("missing 'Name' attribute on '<Constant>' node");

        if(value_value_attrib)
            current_value = xml_stoi(value_value_attrib->value());

        constant_map.emplace(value_name_attrib->value(), current_value);

        ++current_value;
    }
}

static std::pair<std::string, Command>
  parse_command_node(
      const rapidxml::xml_node<>* cmd_node,
      transparent_map<std::string, EntityType>& entities,
      const transparent_map<std::string, shared_ptr<Enum>>& enums)
{
    using namespace rapidxml;

    xml_attribute<>* id_attrib   = cmd_node->first_attribute("ID");
    xml_attribute<>* name_attrib = cmd_node->first_attribute("Name");
    xml_attribute<>* support_attrib = cmd_node->first_attribute("Supported");
    xml_node<>*      args_node   = cmd_node->first_node("Args");

    if(!id_attrib || !name_attrib)
        throw ConfigError("missing 'ID' or 'Name' attribute on '<Command>' node");

    decltype(Command::args) args;

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
            auto allow_text_label_attrib = arg_node->first_attribute("AllowTextLabel");
            auto allow_pointer_attrib= arg_node->first_attribute("AllowPointer");
            auto preserve_case_attrib= arg_node->first_attribute("PreserveCase");
            auto entity_attrib       = arg_node->first_attribute("Entity");
            auto enum_attrib         = arg_node->first_attribute("Enum");

            if(!type_attrib)
                throw ConfigError("missing 'Type' attribute on '<Arg>' node");

            // Cannot build with a initializer list, VS goes mad :(
            Command::Arg arg;
            arg.type = xml_to_argtype(type_attrib->value());
            arg.optional = xml_to_bool(optional_attrib, false);
            arg.is_output = xml_to_bool(out_attrib, false);
            arg.is_ref = xml_to_bool(ref_attrib, false);
            arg.allow_constant = xml_to_bool(allow_const_attrib, arg.is_output? false : true);
            arg.allow_global_var= xml_to_bool(allow_gvar_attrib, true);
            arg.allow_local_var = xml_to_bool(allow_lvar_attrib, true);
            arg.allow_text_label = xml_to_bool(allow_text_label_attrib, false);
            arg.allow_pointer = xml_to_bool(allow_pointer_attrib, false);
            arg.preserve_case = xml_to_bool(preserve_case_attrib, false);
            arg.entity_type = 0;

            if(enum_attrib)
            {
                auto eit = enums.find(enum_attrib->value());
                if(eit != enums.end())
                {
                    arg.enums.emplace_back(eit->second);
                    arg.enums.shrink_to_fit();
                }
            }

            if(entity_attrib)
            {
                auto it = entities.emplace(entity_attrib->value(), EntityType(1 + entities.size())).first;
                arg.entity_type = it->second;
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

static std::pair<std::string, std::vector<const Command*>>
  parse_alternator_node(
      const rapidxml::xml_node<>* alt_node,
      const insensitive_map<std::string, Command>& commands)
{
    using namespace rapidxml;

    std::vector<const Command*> alternatives;

    xml_attribute<>* name_attrib = alt_node->first_attribute("Name");

    if(!name_attrib)
        throw ConfigError("missing 'Name' attribute on '<Alternator>' node");

    for(auto node = alt_node->first_node(); node; node = node->next_sibling())
    {
        xml_attribute<>* attrib = node->first_attribute("Name");

        if(!attrib)
            throw ConfigError("missing 'Name' attribute on '<Alternative>' node");

        auto it = commands.find(attrib->value());
        if(it != commands.end())
            alternatives.emplace_back(std::addressof(it->second));
    }

    return { name_attrib->value(), std::move(alternatives) };
}

Commands Commands::from_xml(const std::string& config_name, const std::vector<fs::path>& xml_list)
{
    using namespace rapidxml;

    // Order by priority (which should be read first).
    constexpr int XML_SECTION_CONSTANTS   = 1;
    constexpr int XML_SECTION_COMMANDS    = 2;
    constexpr int XML_SECTION_ALTERNATORS = 3;

    struct XmlData
    {
        std::string buffer;
        std::unique_ptr<xml_document<>> doc;
    };

    std::vector<XmlData> xml_vector;
    std::vector<std::pair<int, xml_node<>*>> xml_sections;

    insensitive_map<std::string, Command>                       commands;
    insensitive_map<std::string, std::vector<const Command*>>   alternators;
    transparent_map<std::string, EntityType>                    entities;
    transparent_map<std::string, shared_ptr<Enum>>              enums;

    // fundamental enums
    enums.emplace("MODEL", std::make_shared<Enum>(Enum { {}, false, }));
    enums.emplace("CARPEDMODEL", std::make_shared<Enum>(Enum { {}, false, }));

    auto xml_parse = [](const fs::path& path) -> XmlData
    {
        fs::path full_xml_path(path);
        try
        {
            auto opt_buffer = read_file_utf8(full_xml_path);
            if(opt_buffer == nullopt)
                throw ConfigError("failed to read xml {}: {}", full_xml_path.generic_u8string(), "could not open file for reading");

            auto doc = std::make_unique<xml_document<>>(); // buffer should be alive as long as doc
            doc->parse<0>(&(*opt_buffer)[0]); // buffer will get modified here

            return XmlData{ std::move(*opt_buffer), std::move(doc) };
        }
        catch(const rapidxml::parse_error& e)
        {
            throw ConfigError("failed to parse xml {}: {}", full_xml_path.generic_u8string(), e.what());
        }
        catch(const bad_optional_access& e)
        {
            throw ConfigError("failed to read xml {}: {}", full_xml_path.generic_u8string(), e.what());
        }
    };

    for(auto& xml_path : xml_list)
    {
        fs::path path;
        {
            if(!xml_path.is_absolute())
            {
                auto begin = xml_path.begin();
                if(begin != xml_path.end() && (*begin == "." || *begin == ".."))
                    path = xml_path;
                else
                    path = config_path() / config_name / xml_path;
            }
            else
            {
                path = xml_path;
            }
        }

        xml_vector.emplace_back(xml_parse(path));

        if(xml_node<>* root_node = xml_vector.back().doc->first_node("GTA3Script"))
        {
            for(auto node = root_node->first_node(); node; node = node->next_sibling())
            {
                if(!strcmp(node->name(), "Commands"))
                {
                    xml_sections.emplace_back(XML_SECTION_COMMANDS, node);
                }
                else if(!strcmp(node->name(), "Constants"))
                {
                    xml_sections.emplace_back(XML_SECTION_CONSTANTS, node);
                }
                else if(!strcmp(node->name(), "Alternators"))
                {
                    xml_sections.emplace_back(XML_SECTION_ALTERNATORS, node);
                }
            }
        }
    }

    std::stable_sort(xml_sections.begin(), xml_sections.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    for(auto& section_pair : xml_sections)
    {
        xml_node<>* node = section_pair.second;
        if(section_pair.first == XML_SECTION_COMMANDS)
        {
            for(auto cmd_node = node->first_node(); cmd_node; cmd_node = cmd_node->next_sibling())
            {
                if(!strcmp(cmd_node->name(), "Command"))
                {
                    auto cmd_pair = parse_command_node(cmd_node, entities, enums);
                    commands.emplace(std::move(cmd_pair));
                }
            }
        }
        else if(section_pair.first == XML_SECTION_CONSTANTS)
        {
            for(auto const_node = node->first_node(); const_node; const_node = const_node->next_sibling())
            {
                if(!strcmp(const_node->name(), "Enum"))
                {
                    parse_enum_node(enums, const_node);
                }
            }
        }
        else if(section_pair.first == XML_SECTION_ALTERNATORS)
        {
            for(auto alt_node = node->first_node(); alt_node; alt_node = alt_node->next_sibling())
            {
                if(!strcmp(alt_node->name(), "Alternator"))
                {
                    auto alt_pair = parse_alternator_node(alt_node, commands);
                    alternators.emplace(std::move(alt_pair));
                }
            }
        }
    }

    return Commands { std::move(commands), std::move(alternators), std::move(entities), std::move(enums) };
}
