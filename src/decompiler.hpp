// TODO doc comments

#include "stdinc.h"
#include "disassembler.hpp"

struct DecompilerContext;

template<typename T>
std::string decompile_data(const T&, DecompilerContext&);
std::string decompile_data(const DecompiledData&, DecompilerContext&);

struct DecompilerContext
{
private:
    std::vector<DecompiledData> data;

protected:
    friend std::string decompile_data(const DecompiledCommand& ccmd, DecompilerContext& context);

    const Commands& commands;

public:
    DecompilerContext(const Commands& commands, std::vector<DecompiledData> decompiled)
        : commands(commands), data(std::move(decompiled))
    {
    }

    std::string decompile()
    {
        std::string output;
        for(auto& d : this->data)
        {
            output += ::decompile_data(d, *this);
        }
        return output;
    }

};


template<typename T>
inline std::string decompile_data(const T& x, DecompilerContext&)
{
    return x.decompile_data();
}

inline std::string decompile_data(const EOAL&, DecompilerContext&)
{
    return std::string();
}

inline std::string decompile_data(const int8_t& value, DecompilerContext&)
{
    return std::to_string(static_cast<int>(value)); // avoid charish behaviour
}

inline std::string decompile_data(const int16_t& value, DecompilerContext&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const int32_t& value, DecompilerContext&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const float& value, DecompilerContext&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const DecompiledString& str, DecompilerContext&)
{
    std::string output;
    char quotes = 0;

    switch(str.type)
    {
        case CompiledString::Type::TextLabel8:
        case CompiledString::Type::TextLabel16:
            quotes = 0;
            output.reserve(str.storage.size());
            break;
        case CompiledString::Type::StringVar:
        case CompiledString::Type::String128:
            quotes = '"';
            output.reserve(str.storage.size() + 2 + 4); // +2 for quotes; +4 for possible escapes;
            break;
    }

    if(quotes)
        output.push_back(quotes);

    // TODO parse to escape sequences
    for(size_t i = 0; i < str.storage.size(); ++i)
    {
        if(str.storage[i] == '\0') break;
        output.push_back(str.storage[i]);
    }

    if(quotes)
        output.push_back(quotes);

    return output;
}

inline std::string decompile_data(const DecompiledVar& v, DecompilerContext&)
{
    std::string output;
    bool adma = (v.offset % 4) != 0;

    output.push_back(v.global? 'g' : 'l');
    if(adma)
    {
        output.push_back('x');
        output.append(std::to_string(v.offset));
    }
    else
    {
        output.append(std::to_string(v.offset / 4));
    }
    return output;
}

inline std::string decompile_data(const DecompiledVarArray& v, DecompilerContext& context)
{
    std::string output;
    output += decompile_data(v.base, context);
    output.push_back('[');
    output += decompile_data(v.index, context);
    output.push_back(']');
    return output;
}

inline std::string decompile_data(const ArgVariant2& varg, DecompilerContext& context)
{
    return visit_one(varg, [&](const auto& arg) { return ::decompile_data(arg, context); });
}

inline std::string decompile_data(const DecompiledCommand& ccmd, DecompilerContext& context)
{
    std::string output;
    
    bool not_flag = (ccmd.id & 0x8000) != 0;

    auto cmd_name = *context.commands.find_command_name(ccmd.id & 0x7FFF, true);
    
    output.reserve(cmd_name.size() + ccmd.args.size() * 6);

    if(not_flag) output += "NOT ";
    output += cmd_name;
    output.push_back(' ');
    for(auto& arg : ccmd.args)
    {
        output += ::decompile_data(arg, context);
        output.push_back(' ');
    }

    output.push_back('\n');
    return output;
}

inline std::string decompile_data(const DecompiledLabelDef&, DecompilerContext&)
{
    // TODO
    return std::string("\nLABEL________:\n");
}

inline std::string decompile_data(const DecompiledHex& hex, DecompilerContext&)
{
    std::string output;
    output.reserve(sizeof("\nHEX\n") + (hex.data.size() * 3) + sizeof("\nENDHEX\n\n") + 32);

    output += "\nHEX\n";
    for(auto& x : hex.data)
    {
        char buffer[3+1];
        snprintf(buffer, sizeof(buffer), "%.2X ", x);
        output.append(std::begin(buffer), std::end(buffer) - 1);
    }
    output += "\nENDHEX\n\n";
    return output;
}

inline std::string decompile_data(const DecompiledData& data, DecompilerContext& context)
{
    return visit_one(data.data, [&](const auto& data) { return ::decompile_data(data, context); });
}

