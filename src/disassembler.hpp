// TODO doc comments

#pragma once
#include "stdinc.h"
#include "commands.hpp"
#include "compiler.hpp" // for Compiled* types
#include "program.hpp"

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
    enum class ElementType : uint8_t
    {
        None, Int, Float, TextLabel, TextLabel16,
    };

    DecompiledVar base;
    DecompiledVar index;
    int8_t        array_size;
    ElementType   elem_type;
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

// constrat to CompiledScmHeader
struct DecompiledScmHeader
{
    enum class Version : uint8_t
    {
        Liberty,
        Miami,
    };

    Version                               version;
    uint32_t                              size_global_vars_space; // including the 8 bytes of GOTO at the top
    std::vector<std::string>              models;
    uint32_t                              main_size;
    std::vector<uint32_t>                 mission_offsets;

    DecompiledScmHeader(Version version, uint32_t size_globals, std::vector<std::string> models,
        uint32_t main_size, std::vector<uint32_t> mission_offsets) :
        version(version), size_global_vars_space(size_globals), main_size(main_size),
        models(std::move(models)), mission_offsets(std::move(mission_offsets))
    {
    }
};

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
    ProgramContext& program;
    const Commands& commands;

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

    std::vector<uint32_t> local_offsets;

public:
    // undefined behaviour is invoked if data inside `bytecode` is changed while
    // this context object is still alive.
    Disassembler(ProgramContext& program, const Commands& commands,
                        const void* bytecode, size_t size) :
        bytecode((const uint8_t*)(bytecode)), bytecode_size(size),
        program(program), commands(commands)
    {
        // This constructor is **ALWAYS** ran, put all common initialization here.
        this->offset_explored.resize(size);
    }

    Disassembler(ProgramContext& program, const Commands& commands, std::vector<uint8_t> bytecode_) :
        Disassembler(program, commands, bytecode_.data(), bytecode_.size())
    {
        this->bytecode_buffer_ = std::move(bytecode_);
    }

    Disassembler(const Disassembler&) = delete;

    Disassembler(Disassembler&&) = default;

    static optional<Disassembler> from_file(ProgramContext& program, const Commands& commands, const fs::path& path)
    {
        if(auto opt_bytecode = read_file_binary(path))
            return Disassembler(program, commands, *opt_bytecode);
        else
            return nullopt;
    }

    void add_local_offset(size_t offset)
    {
        this->local_offsets.emplace_back(offset);
    }

    void analyze_header(const DecompiledScmHeader& header)
    {
        Expects(this->local_offsets.empty());
        this->local_offsets = header.mission_offsets;

        std::sort(local_offsets.begin(), local_offsets.end());

        for(auto it = local_offsets.rbegin(); it != local_offsets.rend(); ++it)
        {
            this->to_explore.emplace(*it);
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

    optional<DecompiledScmHeader> read_header(DecompiledScmHeader::Version version)
    {
        assert(version == DecompiledScmHeader::Version::Liberty
            || version == DecompiledScmHeader::Version::Miami);

        try
        {
            // TODO check if start of segs is 02 00 01

            auto seg1_offset = 0u;
            auto seg2_offset = fetch_u32(seg1_offset + 3).value();
            auto seg3_offset = fetch_u32(seg2_offset + 3).value();

            uint32_t size_globals = seg2_offset;

            std::vector<std::string> models;
            size_t num_models = static_cast<size_t>((std::max)(1u, fetch_u32(seg2_offset + 8 + 0).value()) - 1);
            models.reserve(num_models);
            for(size_t i = 0; i < num_models; ++i)
            {
                auto model_name = fetch_chars(seg2_offset + 8 + 4 + 24 + (24 * i), 24).value();
                models.emplace_back(std::move(model_name));
            }

            auto main_size = fetch_u32(seg3_offset + 8 + 0).value();
            auto num_missions = fetch_u16(seg3_offset + 8 + 8).value();

            std::vector<uint32_t> mission_offsets;
            for(size_t i = 0; i < num_missions; ++i)
            {
                mission_offsets.emplace_back(fetch_u32(seg3_offset + 8 + 8 + 4 + (4 *i)).value());
            }

            return DecompiledScmHeader { version, size_globals, std::move(models), main_size, std::move(mission_offsets) };
        }
        catch(const bad_optional_access&)
        {
            // the header is incorrect or broken
            return nullopt;
        }
    }


private:

    uint32_t get_absolute_offset(uint32_t addressed_from, int32_t offset)
    {
        if(offset >= 0)
            return offset;

        auto it = std::upper_bound(this->local_offsets.begin(), this->local_offsets.end(), addressed_from);
        if(it != this->local_offsets.begin())
        {
            return *(--it) + (-offset);
        }
        else
        {
            // it's not possible to have a local offset on this location, TODO?
            return -offset;
        }
    }

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

        bool is_switch_start     = false;/* (&command == &this->commands.switch_start()) TODO*/;
        bool is_switch_continued = false;/* (&command == &this->commands.switch_continued()) TODO*/;

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

                uint32_t absolute_value = this->get_absolute_offset(op_offset, value);
                interesting_offsets.emplace(absolute_value);
                label_offsets.emplace(absolute_value);
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
            if(it->type == ArgType::Buffer32)
            {
                if(!fetch_chars(offset, 32))
                    return nullopt;
                offset += 32;
                continue;
            }

            optional<uint8_t> opt_argtype = fetch_u8(offset++);

            if(!opt_argtype)
                return nullopt;

            // Handle III/VC string arguments
            if(*opt_argtype > 0x06 && !this->program.opt.has_text_label_prefix)
            {
                if(it->type == ArgType::TextLabel)
                {
                    offset = offset - 1; // there was no data type, remove one byte
                    if(!fetch_chars(offset, 8))
                        return nullopt;
                    offset += 8;
                    continue;
                }
                return nullopt;
            }

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
                    if(this->program.opt.use_half_float)
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

                //
                // The following cases will not get to run on III/VC
                //

                case 0x09: // Immediate 8-byte string
                    if(!fetch_chars(offset, 8))
                        return nullopt;
                    offset += 8;
                    break;

                case 0x0F: // Immediate 16-byte string
                    if(!fetch_chars(offset, 16))
                        return nullopt;
                    offset += 16;
                    break;

                case 0x0E: // Immediate variable-length string
                {
                    if(auto opt_count = fetch_u8(offset))
                    {
                        if(!fetch_chars(offset+1, *opt_count))
                            return nullopt;
                        offset += *opt_count+1;
                        break;
                    }
                    return nullopt;
                }

                // TODO add rest of data types SA specific

                default:
                    return nullopt;
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
        if(!this->commands.equal(command, this->commands.goto_())
        && !this->commands.equal(command, this->commands.return_())
        && /*!this->commands.equal(command, this->commands.ret()) TODO*/!false
        && !this->commands.equal(command, this->commands.terminate_this_script())
        && /*!this->commands.equal(command, this->commands.terminate_this_custom_script()) TODO*/!false)
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
            if(it->type == ArgType::Buffer32)
            {
                ccmd.args.emplace_back(DecompiledString{ DecompiledString::Type::String128, std::move(*fetch_chars(offset, 32)) });
                offset += 128;
                continue;
            }

            auto datatype = *fetch_u8(offset++);

            // Handle III/VC string arguments
            if(datatype > 0x06 && !this->program.opt.has_text_label_prefix)
            {
                if(it->type == ArgType::TextLabel)
                {
                    offset = offset - 1; // there was no data type, remove one byte
                    ccmd.args.emplace_back(DecompiledString { DecompiledString::Type::TextLabel8, std::move(*fetch_chars(offset, 8)) });
                    offset += 8;
                    continue;
                }
                // code was already analyzed by explore_opcode and it went fine, so...
                Unreachable();
            }

            switch(datatype)
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
                    if(this->program.opt.use_half_float)
                    {
                        ccmd.args.emplace_back(*fetch_i16(offset) / 16.0f);
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


                //
                // The following cases will not get to run on III/VC
                //

                case 0x09: // Immediate 8-byte string
                    ccmd.args.emplace_back(DecompiledString{ DecompiledString::Type::TextLabel8, std::move(*fetch_chars(offset, 8)) });
                    offset += 8;
                    break;

                case 0x0F: // Immediate 16-byte string
                    ccmd.args.emplace_back(DecompiledString{ DecompiledString::Type::TextLabel8, std::move(*fetch_chars(offset, 16)) });
                    offset += 16;
                    break;

                case 0x0E: // Immediate variable-length string
                {
                    auto count = *fetch_u8(offset);
                    ccmd.args.emplace_back(DecompiledString{ DecompiledString::Type::TextLabel8, std::move(*fetch_chars(offset+1, count)) });
                    offset += count + 1;
                    break;
                }

                // TODO add rest of data types SA specific

                default:
                    Unreachable();
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
