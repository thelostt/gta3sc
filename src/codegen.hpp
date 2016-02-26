#pragma once
#include "stdinc.h"

void generate_code(const CompiledData& data, CodeGenerator& codegen);

struct CodeGenerator
{
    const shared_ptr<const Script>  script;
    std::vector<CompiledData>       compiled;
    const SymTable&                 symbols;

    std::unique_ptr<uint8_t[]>      bytecode; // size == script->size
    size_t                          offset;

    CodeGenerator(shared_ptr<const Script> script_, std::vector<CompiledData> compiled, const SymTable& symbols) :
        script(std::move(script_)), compiled(std::move(compiled)), symbols(symbols)
    {
        this->offset = 0;
        this->bytecode.reset(new uint8_t[this->script->size.value()]);
    }

    CodeGenerator(CompilerContext context) : // consumes the context (faster)
        CodeGenerator(std::move(context.script), std::move(context.compiled), context.symbols)
    {}

    /*
    CodeGenerator(const CompilerContext& context) : // does not consume the context (slower)
    CodeGenerator(context.script, context.compiled, context.symbols)
    {}
    */

    void generate()
    {
        assert(this->offset == 0); // Expects
        for(auto& op : this->compiled)
        {
            generate_code(op, *this);
        }
    }

    void emplace_u8(int8_t value)
    {
        assert(this->offset + 1 <= this->script->size); // debug check
        bytecode[this->offset++] = reinterpret_cast<uint8_t&>(value);
    }

    void emplace_u16(int16_t value)
    {
        // TODO maybe optimize, write a entire i16 at a time? is that portable?
        //assert(this->offset + 2 <= this->script->size);
        emplace_u8((value & 0x00FF) >> 0);
        emplace_u8((value & 0xFF00) >> 8);
    }

    void emplace_u32(int32_t value)
    {
        // TODO maybe optimize, write a entire i32 at a time? is that portable?
        //assert(this->offset + 4 <= this->script->size);
        emplace_u8((value & 0x000000FF) >> 0);
        emplace_u8((value & 0x0000FF00) >> 8);
        emplace_u8((value & 0x00FF0000) >> 16);
        emplace_u8((value & 0xFF000000) >> 24);
    }

    void emplace_i8(uint8_t value)
    {
        return emplace_u8(reinterpret_cast<uint8_t&>(value));
    }

    void emplace_i16(uint16_t value)
    {
        return emplace_u16(reinterpret_cast<uint16_t&>(value));
    }

    void emplace_i32(uint32_t value)
    {
        return emplace_u32(reinterpret_cast<uint32_t&>(value));
    }
};

template<typename T>
inline void generate_code(const T& x)
{
    return x.generate_code();
}

inline void generate_code(const EOAL&, CodeGenerator& codegen)
{
    codegen.emplace_u8(0);
}

inline void generate_code(const int8_t& value, CodeGenerator& codegen)
{
    codegen.emplace_u8(4);
    codegen.emplace_i8(value);
}

inline void generate_code(const int16_t& value, CodeGenerator& codegen)
{
    codegen.emplace_u8(5);
    codegen.emplace_i16(value);
}

inline void generate_code(const int32_t& value, CodeGenerator& codegen)
{
    codegen.emplace_u8(1);
    codegen.emplace_i32(value);
}

inline void generate_code(const half& value, CodeGenerator& codegen)
{
    // TODO half
    codegen.emplace_u8(5);
    codegen.emplace_u16(0);
}

inline void generate_code(const float& value, CodeGenerator& codegen)
{
    static_assert(std::numeric_limits<float>::is_iec559
        && sizeof(float) == sizeof(uint32_t), "IEEE 754 floating point expected.");

    codegen.emplace_u8(5);
    codegen.emplace_u32(reinterpret_cast<const uint32_t&>(value));
}

inline void generate_code(const shared_ptr<Label>& label_ptr, CodeGenerator& codegen)
{
    codegen.emplace_u8(1);
    codegen.emplace_i32(label_ptr->global_offset.value());
}

inline void generate_code(const ArgVariant& varg, CodeGenerator& codegen)
{
    return visit_one(varg, [&](const auto& arg) { return ::generate_code(arg, codegen); });
}

inline void generate_code(const CompiledLabelDef&, CodeGenerator&)
{
    // label definitions do not have a physical representation
}

inline void generate_code(const CompiledString&, CodeGenerator&)
{
    // TODO
}

inline void generate_code(const CompiledVar&, CodeGenerator&)
{
    // TODO
}

inline void generate_code(const CompiledCommand& ccmd, CodeGenerator& codegen)
{
    codegen.emplace_u16(ccmd.id);
    for(auto& arg : ccmd.args) ::generate_code(arg, codegen);
}

inline void generate_code(const CompiledData& data, CodeGenerator& codegen)
{
    return visit_one(data.data, [&](const auto& data) { return ::generate_code(data, codegen); });
}