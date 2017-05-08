///
/// Disassembler
///
/// The disassembler is responsible for converting the SCM bytecode into a vector of pseudo-instructions,
/// which can be easily parsed by our program.
///
#pragma once
#include <stdinc.h>
#include "binary_fetcher.hpp"

// contrasts to CompiledVar
struct DecompiledVar
{
    bool     global;
    VarType  type;   // VarType::Float not used, VarType::Int used to denotate both Int and Float
    uint32_t offset; // if global, i*1, if local, i*4;

    bool operator==(const DecompiledVar& rhs) const
    {
        return this->global == rhs.global && this->offset == rhs.offset;
    }
};

// constrats to CompiledVar
struct DecompiledVarArray
{
    enum class ElemType : uint8_t
    {
        None, Int, Float, TextLabel, TextLabel16,
    };

    DecompiledVar base;
    DecompiledVar index;
    uint8_t       array_size;
    ElemType      elem_type;
};

// contrasts to CompiledString
struct DecompiledString
{
    enum class Type : uint8_t
    {
        TextLabel8,
        TextLabel16,
        String128,
        StringVar,
    };

    Type        type;
    std::string storage;
};

// contrasts to ArgVariant
using ArgVariant2 = variant<EOAL, int8_t, int16_t, int32_t, float, DecompiledVar, DecompiledVarArray, DecompiledString>;

// contrasts to CompiledCommand
struct DecompiledCommand
{
    bool                     not_flag;
    const Command*           command;  // never nullptr
    std::vector<ArgVariant2> args;
};

// contrasts to CompiledLabelDef
struct DecompiledLabelDef
{
    size_t offset;      //< Local offset (relative to self-mission-base or main-base)
};

// contrasts to CompiledHex
struct DecompiledHex
{
    std::vector<uint8_t> data;
};

// contrasts to CompiledScmHeader
struct DecompiledScmHeader
{
    enum class Version : uint8_t
    {
        Liberty,
        Miami,
        SanAndreas,
    };

    struct StreamedScript
    {
        std::string name;
        uint32_t    size;
    };

    Version                     version;                //< Version of the header.
    size_t                      code_offset;            //< Offset to after the SCM header.
    uint32_t                    size_global_vars_space; //< (includes the 8 bytes of GOTO at the top)
    std::vector<std::string>    models;                 //< Models header.
    uint32_t                    main_size;              //< Size of the main code segment.
    std::vector<uint32_t>       mission_offsets;        //< Mission header.
    std::vector<StreamedScript> streamed_scripts;       //< Streamed scripts header.

    static optional<DecompiledScmHeader> from_bytecode(const void* bytecode, size_t bytecode_size, Version version);
};

// contrasts to CompiledData
struct DecompiledData
{
    size_t                                                        offset;   //< Local offset of this piece of data
    variant<DecompiledLabelDef, DecompiledCommand, DecompiledHex> data;

    explicit DecompiledData(size_t offset, DecompiledCommand x)
        : offset(offset), data(std::move(x))
    {}

    explicit DecompiledData(size_t offset, std::vector<uint8_t> x)
        : offset(offset), data(DecompiledHex{ std::move(x) })
    {}

    explicit DecompiledData(DecompiledLabelDef x)
        : offset(x.offset), data(std::move(x))
    {}
};


/// Gets the immediate 32 bits value of the value inside the variant, or nullopt if not possible.
template<typename T>
static optional<int32_t> get_imm32(const T&);
static optional<int32_t> get_imm32(const ArgVariant2&);

/// Gets the immediate string / text label value of the value inside the variant, or nullopt if not possible.
template<typename T>
static optional<std::string> get_immstr(const T&);
static optional<std::string> get_immstr(const ArgVariant2&);

/// Returns a vector of { bytecode, size } for each mission in the main.scm buffer.
std::vector<BinaryFetcher> mission_scripts_fetcher(const void* bytecode, size_t bytecode_size,
                                                   const DecompiledScmHeader& header, ProgramContext& program);

/// Returns a vector of { bytecode, size } for each streamed script in the img buffer.
std::vector<BinaryFetcher> streamed_scripts_fetcher(const void* img_bytes, size_t img_size,
                                                    const DecompiledScmHeader& header, ProgramContext& program);

///
class Disassembler
{
public:
    enum class Type
    {
        LinearSweep,            //< Scans the code from top to bottom.
        RecursiveTraversal,     //< Scans the code by following branch instructions.
    };

private:
    ProgramContext&     program;

    /// Bytecode being analyzed.
    BinaryFetcher       bf;

    /// The local offset of the labels in the analyzed bytecode.
    std::set<size_t>    label_offsets;

    /// A bitset of the offsets explored and unexplored. Explored offsets are confirmed to be code.
    dynamic_bitset      offset_explored;

    /// LIFO structure of offsets [mostly confirmed to be code] which still needs to be explored.
    std::stack<size_t>  to_explore;

    /// A hint (for efficient memory allocation) of how many opcodes are in the analyzed bytecode.
    std::size_t         hint_num_ops = 0;

    /// Used internally to process the SWITCH_START/SWITCH_CONTINUED commands.
    std::size_t         switch_cases_left = 0;

    /// Method of disassembling.
    Type                type;

    /// Reference to the Disassembler of the main code segment.
    /// This reference may be pointing to *this.
    Disassembler&       main_asm;

    /// The result of disassemblying.
    std::vector<DecompiledData> decompiled;

    /// OATC header information
    optional<uint16_t> oatc_start;          //< Starting opcode.
    std::vector<const Command*> oatc_table; //< Commands associated with ordinal ids. May contain `nullptr` for unknown commmands.

public:
    /// Constructs assuming `main_asm` to be the main code segment.
    ///
    /// \warning It's undefined what happens with the analyzer if data inside `fetcher.bytecode` is changed while
    /// \warning this Disassembler object is still alive.
    Disassembler(ProgramContext& program, BinaryFetcher fetcher, Disassembler& main_asm, Type type) :
        bf(std::move(fetcher)), program(program), main_asm(main_asm), type(type)
    {
        // This constructor **ALWAYS** run, put all common initialization here.
        this->offset_explored.resize(bf.size);
    }

    /// Constructs assuming `*this` to be the main code segment.
    ///
    /// \warning See the other constructor documentation.
    Disassembler(ProgramContext& program, BinaryFetcher fetcher, Type type) :
        Disassembler(program, std::move(fetcher), *this, type)
    {
    }

    ///
    Disassembler(const Disassembler&) = delete;

    ///
    Disassembler(Disassembler&&) = default;

    /// Is this Disassembler the main code segment?
    bool is_main_segment() const { return this == &main_asm; }

    /// Step 1. Analyze the code.
    void run_analyzer(size_t from_offset = 0);

    /// Step 2. After analyzes, disassembly into a vector of pseudo-instructions.
    void disassembly(size_t from_offset = 0);

    /// Step 3. Get reference to output.
    const std::vector<DecompiledData>& get_data() const&    { return this->decompiled; }
    std::vector<DecompiledData>& get_data() &               { return this->decompiled; }
    std::vector<DecompiledData> get_data() &&               { return std::move(this->decompiled); }

    /// After Step 3. the following is available also.
    /// Gets index on get_data() vector based on a local offset.
    optional<size_t> data_index(uint32_t local_offset) const;

private:

    void analyze();

    void explore(size_t offset);

    /// Attempts to skip a custom header at `offset`.
    /// \returns the offset after the header or `nullopt` if no custom header at `offset`.
    optional<size_t> skip_custom_header(size_t offset) const;

    /// Parses a custom header which the GOTO is at `offset`.
    /// Expects the header at `offset` to exist.
    void parse_custom_header(size_t offset, size_t end_offset);

    /// Tries to explore the `offset` assuming it contains the specified `command`.
    ///
    /// Returns the number of bytes explored (the size of the compiled command),
    /// or `nullopt` if impossible to explore this opcode.
    optional<size_t> explore_opcode(size_t offset, const Command& command, bool not_flag);

    /// Returns a `DecompiledData` containing a `DecompiledCommand` by interpreting `offset`.
    ///
    /// The `offset` **must** have been successfully explored previosly by `explore_opcode`.
    /// For this reason, this call never fails.
    DecompiledData opcode_to_data(size_t& offset);

    /// Gets the command from the opcode id, either using the OATC table or the normal opcode lookup.
    optional<const Command&> command_from_opcode(uint16_t opcode) const;
};


//////////////////////////////

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
    static_assert(std::numeric_limits<float>::is_iec559
        && sizeof(float) == sizeof(uint32_t), "IEEE 754 floating point expected.");
    return reinterpret_cast<const int32_t&>(flt);
}

inline optional<int32_t> get_imm32(const ArgVariant2& varg)
{
    return visit_one(varg, [&](const auto& arg) { return ::get_imm32(arg); });
}

inline optional<std::string> get_immstr(const EOAL&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const DecompiledVar&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const DecompiledVarArray&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const DecompiledString& s)
{
    if(s.type == DecompiledString::Type::StringVar)
    {
        // don't stop on '\0'
        return s.storage;
    }
    else
    {
        auto null_it = std::find(s.storage.begin(), s.storage.end(), '\0');
        return std::string(s.storage.begin(), null_it);
    }
}

inline optional<std::string> get_immstr(const int8_t&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const int16_t&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const int32_t&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const float&)
{
    return nullopt;
}

inline optional<std::string> get_immstr(const ArgVariant2& varg)
{
    return visit_one(varg, [&](const auto& arg) { return ::get_immstr(arg); });
}
