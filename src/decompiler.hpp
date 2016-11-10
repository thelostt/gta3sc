// TODO doc comments

#include <stdinc.h>
#include "disassembler.hpp"

struct DecompilerGTA3Script;

template<typename T>
std::string decompile_data(const T&, DecompilerGTA3Script&);
std::string decompile_data(const DecompiledData&, DecompilerGTA3Script&);

struct DecompilerGTA3Script
{
private:
    std::vector<DecompiledData> data;
    

protected:
    friend std::string decompile_data(const DecompiledCommand& ccmd, DecompilerGTA3Script& context);
    friend std::string decompile_data(const DecompiledLabelDef& label, DecompilerGTA3Script& context);

    const Commands& commands;
    size_t unique_id;           //< Helper for labels
    std::string script_name;    //< Helper for labels

public:
    DecompilerGTA3Script(const Commands& commands, std::vector<DecompiledData> decompiled, size_t unique_id)
        : commands(commands), data(std::move(decompiled)), unique_id(unique_id)
    {
    }

    template<typename OnDecompile>
    void decompile(OnDecompile callback)
    {
        for(auto& d : this->data)
        {
            callback(::decompile_data(d, *this));
        }
    }
};


template<typename T>
inline std::string decompile_data(const T& x, DecompilerGTA3Script&)
{
    return x.decompile_data();
}

inline std::string decompile_data(const EOAL&, DecompilerGTA3Script&)
{
    return std::string();
}

inline std::string decompile_data(const int8_t& value, DecompilerGTA3Script&)
{
    return std::to_string(static_cast<int>(value)); // avoid charish behaviour
}

inline std::string decompile_data(const int16_t& value, DecompilerGTA3Script&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const int32_t& value, DecompilerGTA3Script&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const float& value, DecompilerGTA3Script&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const DecompiledString& str, DecompilerGTA3Script&)
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

inline std::string decompile_data(const DecompiledVar& v, DecompilerGTA3Script&)
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

inline std::string decompile_data(const DecompiledVarArray& v, DecompilerGTA3Script& context)
{
    std::string output;
    output += decompile_data(v.base, context);
    output.push_back('[');
    output += decompile_data(v.index, context);
    output.push_back(']');
    return output;
}

inline std::string decompile_data(const ArgVariant2& varg, DecompilerGTA3Script& context)
{
    return visit_one(varg, [&](const auto& arg) { return ::decompile_data(arg, context); });
}

inline std::string decompile_data(const DecompiledCommand& ccmd, DecompilerGTA3Script& context)
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

    return output;
}

inline std::string decompile_data(const DecompiledLabelDef& label, DecompilerGTA3Script& context)
{
    if(context.script_name.empty())
    {
        if(context.unique_id == 0)
            return fmt::format("\nLABEL_{}:", label.offset);
        else
            return fmt::format("\nLABEL_{}_{}:", context.unique_id, label.offset);
    }
    else
    {
        return fmt::format("\n{}_{}:", context.script_name, label.offset);
    }
}

inline std::string decompile_data(const DecompiledHex& hex, DecompilerGTA3Script&)
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
    output += "\nENDHEX\n";
    return output;
}

inline std::string decompile_data(const DecompiledData& data, DecompilerGTA3Script& context)
{
    return visit_one(data.data, [&](const auto& data) { return ::decompile_data(data, context); });
}

