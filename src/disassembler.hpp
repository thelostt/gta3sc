// TODO doc comments

#pragma once
#include "stdinc.h"
#include "defs/game.hpp"
#include "commands.hpp"
#include "compiler.hpp" // for Compiled* types

//using EOAL = EOAL;

// constrats to CompiledVar
struct DecompiledVar
{
    bool               global;
    uint32_t           offset; // if global, i*1, if local, i*4;

    bool operator==(const DecompiledVar& rhs) const
    {
        return this->global == rhs.global && this->offset == rhs.offset;
    }
};

// constrats to CompiledVar
struct DecompiledVarArray
{
    DecompiledVar base;
    DecompiledVar index;
};

// constrats to CompiledString
using DecompiledString = CompiledString;

// constrats to ArgVariant
using ArgVariant2 = variant<EOAL, int8_t, int16_t, int32_t, float, /*LABEL,*/ DecompiledVar, DecompiledVarArray, DecompiledString>;

// constrats to CompiledCommand
struct DecompiledCommand
{
    uint16_t                 id;
    std::vector<ArgVariant2> args; 
};

// constrat to CompiledLabelDef
struct DecompiledLabelDef
{
    size_t offset;
};

// constrat to CompiledHex
using DecompiledHex = CompiledHex;

// constrat to CompiledData
struct DecompiledData
{
    size_t                                                        offset;
    variant<DecompiledLabelDef, DecompiledCommand, DecompiledHex> data;

    DecompiledData(size_t offset, DecompiledCommand x)
        : offset(offset), data(std::move(x))
    {}

    DecompiledData(size_t offset, std::vector<uint8_t> x)
        : offset(offset), data(DecompiledHex{ std::move(x) })
    {}

    DecompiledData(DecompiledLabelDef x)
        : offset(x.offset), data(std::move(x))
    {}
};


/// Gets the immediate 32 bits value of the value inside the variant, or nullopt if not possible.
///
/// This function should be overloaded/specialized for each possible value in ArgVariant2.
///
template<typename T>
static optional<int32_t> get_imm32(const T&);
static optional<int32_t> get_imm32(const ArgVariant2&);



struct Disassembler
{
private:
    const GameConfig config;
    const Commands&  commands;

    const uint8_t*  bytecode;
    size_t          bytecode_size;

    dynamic_bitset      offset_explored;

    std::set<size_t> label_offsets;

    // must be LIFO
    std::stack<size_t>  to_explore;

    // used when bytecode is allocated by us
    std::vector<uint8_t>    bytecode_buffer_;

    size_t hint_num_ops = 0;

    size_t switch_cases_left = 0;

public:
    // undefined behaviour is invoked if data inside `bytecode` is changed while
    // this context object is still alive.
    Disassembler(GameConfig config, const Commands& commands,
                        const uint8_t* bytecode, size_t size) :
        bytecode(bytecode), bytecode_size(size),
        config(std::move(config)), commands(commands)
    {
        // This constructor is **ALWAYS** ran, put all common initialization here.
        this->offset_explored.resize(size);
    }

    Disassembler(GameConfig config, const Commands& commands, std::vector<uint8_t> bytecode_) :
        Disassembler(std::move(config), commands, bytecode_.data(), bytecode_.size())
    {
        this->bytecode_buffer_ = std::move(bytecode_);
    }

    Disassembler(const Disassembler&) = delete;

    Disassembler(Disassembler&&) = default;

    static Disassembler from_file(GameConfig config, const Commands& commands, const fs::path& path)
    {
        if(auto opt_bytecode = read_file_binary(path))
        {
            return Disassembler(config, commands, *opt_bytecode);
        }
        else
        {
            throw DecompilerError("File {} does not exist", path);
        }
    }




    void run_analyzer()
    {
        this->to_explore.emplace(0x0);
        this->analyze();
    }

    std::vector<DecompiledData> get_data()
    {
        std::vector<DecompiledData> output;

        output.reserve(this->hint_num_ops + 16); // +16 for unknown/hex areas

        for(size_t offset = 0; offset < bytecode_size; )
        {
            if(this->label_offsets.count(offset))
            {
                output.emplace_back(DecompiledLabelDef{ offset });
            }

            if(this->offset_explored[offset])
            {
                output.emplace_back(opcode_to_data(offset));
                // offset was received by ref and mutated ^
            }
            else
            {
                auto begin_offset = offset++;
                for(; offset < bytecode_size; ++offset)
                {
                    // repeat this loop until a label offset or a explored offset is found, then break.
                    //
                    // if a label offset is found, it'll be added at the beggining of the outer for loop,
                    // and then (maybe) this loop will continue.

                    if(this->offset_explored[offset] || this->label_offsets.count(offset))
                        break;
                }

                output.emplace_back(begin_offset, std::vector<uint8_t>(this->bytecode + begin_offset, this->bytecode + offset));
            }
        }

        return output;
    }


private:

    void analyze()
    {
        while(!this->to_explore.empty())
        {
            auto offset = this->to_explore.top();
            this->to_explore.pop();

            this->explore(offset);
        }
    }

    void explore(size_t offset)
    {
        if(offset >= bytecode_size)
        {
            // hm, there's a jump outer of code...
            // ...or we're not detecting flow instructions properly.
            // ??? TODO
            __debugbreak();
            return;
        }

        if(offset_explored[offset])
            return; // already explored

        if(auto opt_cmdid = fetch_u16(offset))
        {
            bool not_flag = (*opt_cmdid & 0x8000) != 0;
            if(auto opt_cmd = this->commands.find_command(*opt_cmdid & 0x7FFF))
            {
                if(explore_opcode(offset, *opt_cmd, not_flag))
                    return;
            }
        }

        // Exploring this byte wasn't quite successful, try the next one.
        this->to_explore.emplace(offset + 1);
    }

    // return is dummy, too lazy to remove all the return nullopt on the function by return false.
    // but check it as if it was a boolean.
    optional<size_t> explore_opcode(size_t op_offset, const Command& command, bool not_flag)
    {
        // delay addition of offsets into `this->to_explore`, the opcode may be illformed while we're analyzing it.
        std::stack<size_t> interesting_offsets;

        size_t offset = op_offset + 2;

        bool stop_it = false;

        bool is_switch_start     = (&command == &this->commands.switch_start());
        bool is_switch_continued = (&command == &this->commands.switch_continued());

        size_t argument_id = 0;

        auto check_for_imm32 = [&](auto value, const Command::Arg& arg)
        {
            if(is_switch_start && argument_id == 1)
            {
                this->switch_cases_left = value;
            }

            if(arg.type == ArgType::Label)
            {
                if(is_switch_start || is_switch_continued)
                {
                    if(this->switch_cases_left == 0)
                        return; // don't take offset
                    
                    if(is_switch_start && argument_id != 3) // not default label
                        --this->switch_cases_left;
                }

                // TODO treat negative offsets properly (relative to mission base, etc)
                interesting_offsets.push(value < 0? -value : value);
                label_offsets.emplace(value < 0? -value : value);
            }
        };

        if(is_switch_start)
        {
            // We need this set to 0 since the switch cases argument mayn't
            // be a constant (ill-formed, but game executes).
            this->switch_cases_left = 0;
        }

        // TODO optimize out fetches here to just check offset + N

        for(auto it = command.args.begin();
            !stop_it && it != command.args.end();
            (it->optional? it : ++it), ++argument_id)
        {
            if(it->type == ArgType::Buffer128)
            {
                if(!fetch_chars(offset, 128))
                    return nullopt;
                offset += 128;
                continue;
            }

            optional<uint8_t> opt_argtype = fetch_u8(offset++);

            if(!opt_argtype)
                return nullopt;

            switch(*opt_argtype)
            {
                case 0x00: // EOA (end of args)
                    if(!it->optional)
                        return nullopt;
                    stop_it = true;
                    break;

                case 0x01: // Int32
                    if(auto opt = fetch_i32(offset))
                    {
                        check_for_imm32(*opt, *it);
                        offset += sizeof(int32_t);
                        break;
                    }
                    return nullopt;

                case 0x04: // Int8
                    if(auto opt = fetch_i8(offset))
                    {
                        check_for_imm32(*opt, *it);
                        offset += sizeof(int8_t);
                        break;
                    }
                    return nullopt;

                case 0x05: // Int16
                    if(auto opt = fetch_i16(offset))
                    {
                        check_for_imm32(*opt, *it);
                        offset += sizeof(int16_t);
                        break;
                    }
                    return nullopt;

                case 0x02: // Global Int/Float Var
                case 0x03: // Local Int/Float Var
                    if(!fetch_u16(offset))
                        return nullopt;
                    offset += sizeof(uint16_t);
                    break;

                case 0x06: // Float
                    if(this->config.use_half_float)
                    {
                        if(!fetch_i16(offset))
                            return nullopt;
                        offset += sizeof(int16_t);
                    }
                    else
                    {
                        if(!fetch_u32(offset))
                            return nullopt;
                        offset += sizeof(uint32_t);
                    }
                    break;

                default: // TODO add rest of data types SA specific
                    if(!this->config.has_text_label_prefix)
                    {
                        if(it->type == ArgType::TextLabel)
                        {
                            offset = offset - 1; // there was no data type, remove one byte
                            if(!fetch_chars(offset, 8))
                                return nullopt;
                            offset += 8;
                            break;
                        }
                        return nullopt;
                    }
                    else
                    {
                        return nullopt;
                    }
                    break;
            }
        }

        // OK, opcode is not ill formed, we can push up the new offsets to explore
        while(!interesting_offsets.empty())
        {
            this->to_explore.emplace(interesting_offsets.top());
            interesting_offsets.pop();
        }

        // add next instruction as the next thing to be explored, if this isn't a instruction that
        // terminates execution or jumps unconditionally to another offset.
        // TODO would be nice if this was actually configurable.
        if(&command != &this->commands.goto_()
        && &command != &this->commands.return_()
        && &command != &this->commands.ret()
        && &command != &this->commands.terminate_this_script())
        // TODO more
        {
            if((is_switch_start || is_switch_continued) && this->switch_cases_left == 0)
            {
                // we are at the last SWITCH_START/SWITCH_CONTINUED command, after this, the game will take a branch.
            }
            else
            {
                this->to_explore.emplace(offset);
            }
        }

        // mark this area as explored
        for(size_t i = op_offset; i < offset; ++i)
            this->offset_explored[i] = true;

        ++this->hint_num_ops;

        return offset - op_offset;
    }

    // this function should not really fail.
    // before it runs, `explore_opcode` ran, meaning everything is alright.
    DecompiledData opcode_to_data(size_t& offset)
    {
        auto cmdid     = *fetch_u16(offset);
        bool not_flag  = (cmdid & 0x8000) != 0;
        const Command& command = *this->commands.find_command(cmdid & 0x7FFF);

        bool stop_it = false;

        DecompiledCommand ccmd;
        ccmd.id = cmdid;

        auto start_offset = offset;
        offset = offset + 2;

        for(auto it = command.args.begin();
            !stop_it && it != command.args.end();
            it->optional? it : ++it)
        {
            if(it->type == ArgType::Buffer128)
            {
                ccmd.args.emplace_back(DecompiledString{ DecompiledString::Type::String128, std::move(*fetch_chars(offset, 128)) });
                offset += 128;
                continue;
            }

            switch(*fetch_u8(offset++))
            {
                case 0x00:
                    ccmd.args.emplace_back(EOAL{});
                    stop_it = true;
                    break;

                case 0x01: // Int32
                {
                    auto i32 = *fetch_i32(offset);
                    offset += sizeof(int32_t);
                    ccmd.args.emplace_back(i32);
                    break;
                }

                case 0x04: // Int8
                {
                    auto i8 = *fetch_i8(offset);
                    offset += sizeof(int8_t);
                    ccmd.args.emplace_back(i8);
                    break;
                }

                case 0x05: // Int16
                {
                    auto i16 = *fetch_i16(offset);
                    offset += sizeof(int16_t);
                    ccmd.args.emplace_back(i16);
                    break;
                }

                case 0x02: // Global Int/Float Var
                    ccmd.args.emplace_back(DecompiledVar { true, *fetch_u16(offset) });
                    offset += sizeof(uint16_t);
                    break;

                case 0x03: // Local Int/Float Var
                    ccmd.args.emplace_back(DecompiledVar { false, *fetch_u16(offset) * 4u });
                    offset += sizeof(uint16_t);
                    break;

                case 0x06: // Float
                    if(this->config.use_half_float)
                    {
                        auto from_half = [](int16_t value) -> float {
                            if(!value) return 0.0f;
                            return value / 16.0f;
                        };

                        ccmd.args.emplace_back(from_half(*fetch_i16(offset)));
                        offset += sizeof(int16_t);
                    }
                    else
                    {
                        static_assert(std::numeric_limits<float>::is_iec559
                            && sizeof(float) == sizeof(uint32_t), "IEEE 754 floating point expected.");

                        ccmd.args.emplace_back(reinterpret_cast<const float&>(*fetch_u32(offset)));
                        offset += sizeof(uint32_t);
                    }
                    break;

                default: // TODO add rest of data types SA specific
                    if(!this->config.has_text_label_prefix)
                    {
                        if(it->type == ArgType::TextLabel)
                        {
                            offset = offset - 1; // there was no data type, remove one byte
                            ccmd.args.emplace_back(DecompiledString { DecompiledString::Type::TextLabel8, std::move(*fetch_chars(offset, 8)) });
                            offset += 8;
                            break;
                        }
                        Unreachable();
                    }
                    else
                    {
                        Unreachable();
                    }
                    break;
            }
        }

        return DecompiledData(start_offset, std::move(ccmd));
    }











    optional<uint8_t> fetch_u8(size_t offset)
    {
        if(offset + 1 <= bytecode_size)
        {
             return this->bytecode[offset];
        }
        return nullopt;
    }

    optional<uint16_t> fetch_u16(size_t offset)
    {
        if(offset + 2 <= bytecode_size)
        {
            return uint16_t(this->bytecode[offset+0]) << 0
                 | uint16_t(this->bytecode[offset+1]) << 8;
        }
        return nullopt;
    }

    optional<uint32_t> fetch_u32(size_t offset)
    {
        if(offset + 4 <= bytecode_size)
        {
            return uint32_t(this->bytecode[offset+0]) << 0
                 | uint32_t(this->bytecode[offset+1]) << 8
                 | uint32_t(this->bytecode[offset+2]) << 16
                 | uint32_t(this->bytecode[offset+3]) << 24;
        }
        return nullopt;
    }

    optional<int8_t> fetch_i8(size_t offset)
    {
        if(auto opt = fetch_u8(offset))
        {
            return reinterpret_cast<int8_t&>(*opt);
        }
        return nullopt;
    }

    optional<int16_t> fetch_i16(size_t offset)
    {
        if(auto opt = fetch_u16(offset))
        {
            return reinterpret_cast<int16_t&>(*opt);
        }
        return nullopt;
    }

    optional<int32_t> fetch_i32(size_t offset)
    {
        if(auto opt = fetch_u32(offset))
        {
            return reinterpret_cast<int32_t&>(*opt);
        }
        return nullopt;
    }

    optional<char*> fetch_chars(size_t offset, size_t count, char* output)
    {
        if(offset + count <= bytecode_size)
        {
            std::strncpy(output, reinterpret_cast<const char*>(&this->bytecode[offset]), count);
            return output;
        }
        return nullopt;
    }

    optional<std::string> fetch_chars(size_t offset, size_t count)
    {
        std::string str(count, '\0');
        if(fetch_chars(offset, count, &str[0]))
            return str;
        return nullopt;
    }
};



inline optional<int32_t> get_imm32(const EOAL&)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const DecompiledVar&)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const DecompiledVarArray&)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const DecompiledString&)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const int8_t& i8)
{
    return static_cast<int32_t>(i8);
}

inline optional<int32_t> get_imm32(const int16_t& i16)
{
    return static_cast<int32_t>(i16);
}

inline optional<int32_t> get_imm32(const int32_t& i32)
{
    return static_cast<int32_t>(i32);
}

inline optional<int32_t> get_imm32(const float& flt)
{
    // TODO floating point format static assert
    return reinterpret_cast<const int32_t&>(flt);
}

inline optional<int32_t> get_imm32(const ArgVariant2& varg)
{
    return visit_one(varg, [&](const auto& arg) { return ::get_imm32(arg); });
}
